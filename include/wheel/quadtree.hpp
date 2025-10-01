// TODO: try_merge performance
#pragma once

#include <wheel/geometry.hpp>
#include <wheel/log.hpp>

#include <array>
#include <vector>
#include <unordered_set>
#include <memory>

namespace wheel {

// T should be hashable(stored in std::unordered_set for update)
template <typename T, typename GetRect>
class QuadTree {
public:
    QuadTree(int threshold = 16, int max_depth = 8)
        : threshold_(threshold), max_depth_(max_depth), root_(std::make_unique<Node>()) {}
    void set_rect(const Rect<float>& rect) { node_rect_ = rect; }
    void set_get_rect(GetRect get_rect) { get_rect_ = get_rect; }

    void add(const T& value) {
        if (!values_.count(value)) {
            values_.emplace(value);
            return add(root_.get(), node_rect_, get_rect_(value), value, 0);
        }
    }

    void remove(const T& value) {
        if (values_.count(value)) {
            values_.erase(value);
            remove(root_.get(), node_rect_, get_rect_(value), value);
        }
    }

    // TODO: optimize
    std::vector<T> query(const T& value) const {
        auto values = query(get_rect_(value));
        values.erase(std::remove(values.begin(), values.end(), value), values.end());
        return values;
    }

    std::vector<T> query(const Rect<float>& rect) const {
        auto values = std::vector<T>{};
        query_(root_.get(), node_rect_, rect, values);
        return values;
    }

    void update() {
        root_ = std::make_unique<Node>();
        for (const auto& value : values_) {
            add(root_.get(), node_rect_, get_rect_(value), value, 0);
        }
    }

    void print() {
        print(root_.get(), node_rect_, "");
    }

    std::vector<std::pair<T, T>> query_all() const {
        auto intersections = std::vector<std::pair<T, T>>{};
        query_all(root_.get(), intersections);
        return intersections;
    }

    size_t size() const { return values_.size(); }

    void clear() {
        root_ = std::make_unique<Node>();
        values_.clear();
    }

private:
    struct Node {
        std::array<std::unique_ptr<Node>, 4> children;
        std::vector<T> values;
    };

    Rect<float> node_rect_;
    const int threshold_;
    const int max_depth_;
    std::unique_ptr<Node> root_;
    GetRect get_rect_;
    std::unordered_set<T> values_;

    bool is_leaf(const Node* node) const {
        return !static_cast<bool>(node->children[0]);
    }

    Rect<float> calc_child_rect(const Rect<float>& rect, int i) const {
        auto [x, y] = rect.left_top();
        auto [w, h] = rect.size() / 2;
        float x0, y0;
        switch (i) {
            // left top
            case 0: x0 = x, y0 = y; break;
            // right top
            case 1: x0 = x + w, y0 = y; break;
            // left bottom
            case 2: x0 = x, y0 = y + h; break;
            // right bottom
            case 3: x0 = x + w, y0 = y + h; break;
            // wrong
            default: Log::assert_(false, "QuadTree::calc_child_rect(): invalid index");
        }
        return Rect<float>{x0, y0, x0 + w, y0 + h};
    }

    int get_quadrant(const Rect<float>& node_rect, const Rect<float>& input_rect) const {
        auto [x, y] = node_rect.center();
        auto [x0, y0, x1, y1] = input_rect;

        // left
        if (x1 < x) {
            // top
            if (y1 < y) {
                return 0;
            // bottom
            } else if (y0 > y) {
                return 2;
            }
        // right
        } else if (x0 > x) {
            // top
            if (y1 < y) {
                return 1;
            // bottom
            } else if (y0 > y) {
                return 3;
            }
        }

        // not entirely contained in any of the quadrants
        return -1;
    }

    void add(Node* node, const Rect<float>& node_rect, const Rect<float>& input_rect, const T& value, int depth) {
        // Log::assert_(node_rect.contains(input_rect), "QuadTree::add(): input_rect is not contained in node_rect");

        if (is_leaf(node)) {
            if (depth >= max_depth_ || node->values.size() < threshold_) {
                node->values.emplace_back(value);
            } else {
                split(node, node_rect);
                add(node, node_rect, input_rect, value, depth);
            }
        } else {
            auto i = get_quadrant(node_rect, input_rect);
            if (i != -1) {
                add(node->children[i].get(), calc_child_rect(node_rect, i), input_rect, value, depth + 1);
            } else {
                node->values.emplace_back(value);
            }
        }
    }

    void split(Node* node, const Rect<float>& node_rect) {
        for (auto& child : node->children) {
            child = std::make_unique<Node>();
        }
        auto new_values = std::vector<T>{};
        for (const auto& value : node->values) {
            int i = get_quadrant(node_rect, get_rect_(value));
            if (i != -1) {
                node->children[i]->values.emplace_back(value);
            } else {
                new_values.emplace_back(value);
            }
        }
        node->values = std::move(new_values);
    }

    // return true to indicate that the leaf node has been deleted.
    bool remove(Node* node, const Rect<float>& node_rect, const Rect<float>& input_rect, const T& value) {
        if (is_leaf(node)) {
            remove_value(node, value);
            return true;
        } else {
            int i = get_quadrant(node_rect, input_rect);
            if (i != -1) {
                if (remove(node->children[i].get(), calc_child_rect(node_rect, i), input_rect, value)) {
                    return try_merge(node);
                }
            } else {
                remove_value(node, value);
                return false;
            }
        }
        return false;
    }

    void remove_value(Node* node, const T& value) {
        auto it = std::find(node->values.begin(), node->values.end(), value);
        Log::assert_(it != node->values.end(), "QuadTree::remove_value(): can't find value!");
        *it = std::move(node->values.back());
        node->values.pop_back();
    }

    bool try_merge(Node* node) {
        Log::assert_(!is_leaf(node), "QuadTree::try_merge(): only interior nodes can be merged!");
        int size = node->values.size();
        for (const auto& child : node->children) {
            if (!is_leaf(child.get())) {
                return false;
            }
            size += child->values.size();
        }
        if (size <= threshold_) {
            node->values.reserve(size);
            for (auto& child : node->children) {
                for (const auto& value : child->values) {
                    node->values.emplace_back(value);
                }
                child.reset();
            }
            return true;
        }

        return false;
    }

    void query_(Node* node, const Rect<float>& node_rect, const Rect<float>& input_rect, std::vector<T>& values) const {
        // Log::assert_(input_rect.is_overlapping(node_rect), "QuadTree::query(): input_rect is not overlapping with node_rect");

        for (const auto& value : node->values) {
            if (input_rect.is_overlapping(get_rect_(value))) {
                values.emplace_back(value);
            }
        }
        if (!is_leaf(node)) {
            for (int i = 0; i < 4; i++) {
                auto child_rect = calc_child_rect(node_rect, i);
                if (input_rect.is_overlapping(child_rect)) {
                    query_(node->children[i].get(), child_rect, input_rect, values);
                }
            }
        }
    }

    void print(Node* node, const Rect<float>& node_rect, const std::string& indent) {
        if (node == nullptr) {
            return;
        }

        // Print the node
        std::cout << indent << node_rect << " values:[";
        for (const auto& value : node->values) {
            std::cout << get_rect_(value) << " ";
        }
        std::cout << "]\n";

        // If this is not a leaf node, print its children
        if (!is_leaf(node)) {
            for (int i = 0; i < 4; i++) {
                std::cout << indent << "Child " << i << ":\n";
                auto child_rect = calc_child_rect(node_rect, i);
                print(node->children[i].get(), child_rect, indent + "  ");
            }
        }
    }

    void query_all(Node* node, std::vector<std::pair<T, T>>& intersections) const {
        for (int i = 1; i < node->values.size(); i++) {
            for (int j = 0; j < i; j++) {
                const auto& vi = node->values[i], vj = node->values[j];
                if (get_rect_(vi).is_overlapping(get_rect_(vj))) {
                    intersections.emplace_back(vi, vj);
                }
            }
        }
        if (!is_leaf(node)) {
            for (const auto& child : node->children) {
                for (const auto& value : node->values) {
                    query_children(child.get(), value, intersections);
                }
            }
            for (const auto& child : node->children) {
                query_all(child.get(), intersections);
            }
        }
    }

    void query_children(Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const {
        for (const auto& other : node->values) {
            if (get_rect_(value).is_overlapping(get_rect_(other))) {
                intersections.emplace_back(value, other);
            }
        }
        if (!is_leaf(node)) {
            for (const auto& child : node->children) {
                query_children(child.get(), value, intersections);
            }
        }
    }
};

}  // namespace wheel
