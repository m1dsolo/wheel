// TODO: try_merge performance
#pragma once

#include <wheel/geometry.hpp>
#include <wheel/log.hpp>

#include <array>
#include <vector>
#include <memory>

namespace wheel {

template <typename T, typename GetRect>
class QuadTree {
public:
    QuadTree(const Rect<float>& rect, const GetRect& get_rect, int threshold = 16, int max_depth = 8)
        : node_rect_(rect), get_rect_(get_rect), threshold_(threshold), max_depth_(max_depth), root_(std::make_unique<Node>()) {}

    void add(const T& value) {
        return add(root_.get(), node_rect_, get_rect(value), value, 0);
    }

    void remove(const T& value) {
        remove(root_.get(), node_rect_, get_rect(value), value);
    }

    std::vector<Rect<float>> query(const T& value) const {
        auto values = std::vector<T>{};
        query(root_.get(), node_rect_, get_rect(value), value);
        return values;
    }

    std::vector<std::pair<T, T>> find_all_intersections() {
        auto intersections = std::vector<std::pair<T, T>>{};
        find_all_intersections(root_.get(), intersections);
        return intersections;
    }

private:
    struct Node {
        std::array<std::unique_ptr<Node>, 4> children;
        std::vector<T> values;
    };

    const Rect<float>& node_rect_;
    const int threshold_;
    const int max_depth_;
    std::unique_ptr<Node> root_;
    GetRect get_rect_;

    bool is_leaf(const Node* node) const {
        return !static_cast<bool>(node->children[0]);
    }

    Rect<float> calc_child_rect(const Rect<float>& rect, int i) const {
        auto [x, y] = rect.left_top();
        auto [w, h] = rect.size() / 2;
        switch (i) {
            // left top
            case 0: return Rect<float>{x, y, w, h};
            // right top
            case 1: return Rect<float>{x + w, y, w, h};
            // left bottom
            case 2: return Rect<float>{x, y + h, w, h};
            // right bottom
            case 3: return Rect<float>{x + w, y + h, w, h};
            // wrong
            default: Log::assert(false, "calc_child_rect invalid index");
        }
    }

    int get_quadrant(const Rect<float>& node_rect, const Rect<float>& input_rect) const {
        auto [x, y] = node_rect.center();
        // left
        if (input_rect.x1 < x) {
            // top
            if (input_rect.y1 < y) {
                return 0;
            // bottom
            } else if (input_rect.y0 >= y) {
                return 2;
            }
        // right
        } else {
            // top
            if (input_rect.y1 < y) {
                return 1;
            // bottom
            } else if (input_rect.y0 >= y) {
                return 3;
            }
        }

        // not entirely contained in any of the quadrants
        return -1;
    }

    void add(Node* node, const Rect<float>& node_rect, const Rect<float>& input_rect, const T& value, int depth) {
        Log::assert(node, node_rect.contains(input_rect));
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
        auto new_values = std::vector<Rect<float>>{};
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
                if (remove(node->children[i].get(), calc_child_rect(node_rect, i), input_rect)) {
                    return try_merge(node);
                }
            } else {
                remove_value(node, value);
                return false;
            }
        }
    }

    void remove_value(Node* node, const T& value) {
        auto it = std::find(node->values.begin(), node->values.end(), value);
        Log::assert(it != node->values.end(), "remove_value can't find value!");
        *it = std::move(node->values.back());
        node->values.pop_back();
    }

    bool try_merge(Node* node) {
        Log::assert(node && !is_leaf(node), "only interior nodes can be merged!");
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

    void query(Node* node, const Rect<float>& node_rect, const Rect<float>& input_rect, std::vector<T>& values) const {
       Log::assert(node && input_rect.is_overlapping(node_rect_), "query get_rect(value) is not overlapping with node_rect_");
        for (const auto& value : node->values) {
            if (input_rect.is_overlapping(value)) {
                values.emplace_back(value);
            }
        }
        if (!is_leaf(node)) {
            for (int i = 0; i < 4; i++) {
                auto child_rect = calc_child_rect(node_rect_, i);
                if (input_rect.is_overlapping(child_rect)) {
                    query(node, child_rect, input_rect, values);
                }
            }
        }
    }

    void find_all_intersections(Node* node, std::vector<std::pair<T, T>>& intersections) const {
        for (int i = 0; i < node->values.size(); i++) {
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
                    find_descendant_intersections(child.get(), value, intersections);
                }
            }
            for (const auto& child : node->children) {
                find_all_intersections(child.get(), intersections);
            }
        }
    }

    void find_descendant_intersections(Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) {
        for (const auto& other : node->values) {
            if (get_rect_(value).is_overlapping(get_rect_(other))) {
                intersections.emplace_back(value, other);
            }
        }
        if (!is_leaf(node)) {
            for (const auto& child : node->children) {
                find_descendant_intersections(child.get(), value, intersections);
            }
        }
    }
};

}  // namespace wheel
