import cmd
import subprocess
import threading
import sys
from pathlib import Path


class VoiceAssistantCLI(cmd.Cmd):

    intro = """
=========================================
 AI Voice Assistant CLI
=========================================
Ketik 'help' untuk melihat daftar perintah.
"""
    prompt = "> "

    def __init__(self):
        super().__init__()
        self.server_process = None

    def read_server_output(self):
        """Membaca semua print() dari websocket.py"""
        while True:
            if self.server_process is None:
                break
            line = self.server_process.stdout.readline()

            if not line:
                break
                
            print(f"\n[SERVER] {line.strip()}")
            print(self.prompt, end="", flush=True)

    def start_server(self):
        if self.server_process and self.server_process.poll() is None:
            print("[INFO] Server sudah berjalan.")
            return

        websocket_file = Path(__file__).parent / "websocket.py"

        self.server_process = subprocess.Popen(
            [sys.executable, "-u", str(websocket_file)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            stdin=subprocess.DEVNULL,
            text=True,
            bufsize=1
        )

        threading.Thread(
            target=self.read_server_output,
            daemon=True
        ).start()

        print("[INFO] Menjalankan websocket.py...")

    def stop_server(self):
        if self.server_process is None:
            print("[INFO] Server belum berjalan.")
            return

        if self.server_process.poll() is None:
            self.server_process.terminate()
            self.server_process.wait()

        self.server_process = None

        print("[INFO] Server dihentikan.")

    def do_server(self, arg):
        arg = arg.lower().strip()

        if arg == "on":
            self.start_server()
        elif arg == "off":
            self.stop_server()
        else:
            print("Gunakan:")
            print("server on")
            print("server off")

    def do_status(self, arg):
        if self.server_process and self.server_process.poll() is None:
            print("Server : ON")
        else:
            print("Server : OFF")

    def do_exit(self, arg):
        self.stop_server()
        return True

    def do_quit(self, arg):
        return self.do_exit(arg)

    def emptyline(self):
        pass

if __name__ == "__main__":
    VoiceAssistantCLI().cmdloop()