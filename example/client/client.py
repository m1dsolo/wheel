import socket
import sys
import threading

from textual.app import App, ComposeResult
from textual.widgets import (
    Footer,
    Header,
    Input,
    RichLog,
)

class ChatApp(App[None]):

    CSS_PATH = 'client.tcss'
    BINDINGS = [
        ('d', 'toggle_dark', 'Toggle dark mode'),
        ('q', 'quit', 'Quit'),
    ]
    TITLE = 'Chat'

    def __init__(self, ip: str, port: int):
        super().__init__()

        self.ip = ip
        self.port = port
        self.running = True

        # 1. socket
        self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP

        # 2. connect
        self.fd.connect((ip, port))

        # 3. communicate
        threading.Thread(target=self.worker_recv, daemon=True).start()

        self.lock = threading.Lock()

    def worker_recv(self):
        while True:
            try:
                data = self.fd.recv(1024)
                if not data:
                    self.running = False
                    break
                self.write(data.decode())
            except socket.error as e:
                self.running = False
                break

    def compose(self) -> ComposeResult:
        yield Header()

        self.chat_log = RichLog(id='chat-log')
        self.chat_input = Input(placeholder='input', id='chat-input')

        yield self.chat_log
        yield self.chat_input

        yield Footer()

    def on_mount(self) -> None:
        self.chat_input.focus()

    async def on_input_submitted(self, message: Input.Submitted) -> None:
        self.chat_input.value = ''

        s = message.value
        if not self.running or s == '':
            self.exit(message="connection closed")
            # return
        
        self.fd.sendall(s.encode())

    def write(self, s):
        self.lock.acquire()
        if s[-1] == '\n':
            s = s[:-1]
        self.chat_log.write(s)
        self.lock.release()

    def action_toggle_dark(self) -> None:
        '''An action to toggle dark mode.'''
        self.dark = not self.dark

    def __del__(self):
        self.running = False
        self.fd.close()

if __name__ == '__main__':
    assert len(sys.argv) == 3
    ChatApp(sys.argv[1], int(sys.argv[2])).run()

