#!/usr/bin/env python3
# ---@IncursioHack---
import os
import subprocess
import shutil
from pathlib import Path
from http.server import HTTPServer, SimpleHTTPRequestHandler
from datetime import datetime

# --- 1. Check if PlatformIO CLI is installed ---
if shutil.which("pio") is None:
    print("PlatformIO not found. Installing via pip --user...")
    subprocess.run(["python3", "-m", "pip", "install", "--user", "platformio"], check=True)
    os.environ["PATH"] = f"{os.environ['HOME']}/.local/bin:{os.environ['PATH']}"

# --- 2. Read default_envs from platformio.ini as raw text ---
ini_file = Path("platformio.ini")
if not ini_file.exists():
    print("platformio.ini not found")
    exit(1)

envs = []
in_platformio_section = False
in_default_envs = False

with open(ini_file) as f:
    for line in f:
        line_strip = line.strip()
        if line_strip.startswith("[platformio]"):
            in_platformio_section = True
            in_default_envs = False
            continue
        elif line_strip.startswith("[") and line_strip != "[platformio]":
            in_platformio_section = False
            in_default_envs = False
        if in_platformio_section:
            if line_strip.startswith("default_envs"):
                in_default_envs = True
                parts = line_strip.split("=", 1)
                if len(parts) > 1:
                    val = parts[1].strip()
                    if val:
                        envs.append(val)
            elif in_default_envs:
                # Stop if a line looks like a config directive instead of a board
                if "=" in line_strip:
                    break
                if line_strip and not line_strip.lower().startswith(";uncomment"):
                    envs.append(line_strip)

# --- 3. Clean env names and remove leading ';' ---
clean_envs = []
for e in envs:
    e_clean = e.lstrip("; ").strip()
    # Skip lines that are not board names (simple heuristic)
    if e_clean and not e_clean.lower().startswith("uncomment") and not e_clean.lower().startswith("boards") and not e_clean.lower().startswith("build_cache_dir") and not e_clean.lower().startswith("cache_dir"):
        clean_envs.append(e_clean)

if not clean_envs:
    print("No environments (boards) found in platformio.ini")
    exit(1)

# --- 4. Display menu for environment selection ---
print("Select the board/environment for build:")
for i, e in enumerate(clean_envs):
    print(f"{i}) {e}")

try:
    choice = int(input("Number: "))
    env = clean_envs[choice]
except (ValueError, IndexError):
    print("Invalid choice")
    exit(1)

print(f"Building for {env}...")

# --- 5. Run PlatformIO build ---
res = subprocess.run(["pio", "run", "-e", env])
if res.returncode != 0:
    print("Build failed")
    exit(1)

# --- 6. Copy firmware.bin to temporary folder ---
bin_path = Path(f".pio/build/{env}/firmware.bin")
if not bin_path.exists():
    print(f"Error: {bin_path} not found")
    exit(1)

tmp_dir = Path("/tmp/fwtest")
tmp_dir.mkdir(exist_ok=True)
dest = tmp_dir / "firmware.bin"
shutil.copy(bin_path, dest)

mod_time = datetime.fromtimestamp(dest.stat().st_mtime)
print(f"Firmware ready: {dest} (last modified: {mod_time})")

# --- 7. Start HTTP server with download support and directory listing with date/time ---
os.chdir(tmp_dir)
print("HTTP server started at /tmp/fwtest:8000")
print("Open the PORTS tab in Codespaces and set port 8000 to Public")

class BinDownloadHandler(SimpleHTTPRequestHandler):
    """HTTP handler that forces .bin download and shows last-modified date in directory listing."""

    def end_headers(self):
        if self.path.endswith(".bin"):
            file_path = Path(self.translate_path(self.path))
            if file_path.exists():
                last_modified = file_path.stat().st_mtime
                self.send_header("Last-Modified", self.date_time_string(last_modified))
            self.send_header("Content-Disposition", f"attachment; filename={os.path.basename(self.path)}")
            self.send_header("Content-Type", "application/octet-stream")
        super().end_headers()

    def list_directory(self, path):
        """Override directory listing to include last-modified date."""
        try:
            entries = sorted(os.listdir(path))
        except OSError:
            self.send_error(404, "No permission to list directory")
            return None

        r = []
        displaypath = os.path.relpath(path, Path.cwd())
        r.append(f"<html><head><title>Directory listing for {displaypath}</title></head>")
        r.append("<body>")
        r.append(f"<h2>Directory listing for {displaypath}</h2>")
        r.append("<hr><ul>")
        for name in entries:
            fullname = os.path.join(path, name)
            display_name = name
            link_name = name
            if os.path.isdir(fullname):
                display_name += "/"
                link_name += "/"
            mod_time = datetime.fromtimestamp(os.stat(fullname).st_mtime).strftime("%Y-%m-%d %H:%M:%S")
            r.append(f'<li><a href="{link_name}">{display_name}</a> (last modified: {mod_time})</li>')
        r.append("</ul><hr></body></html>")
        encoded = "\n".join(r).encode("utf-8", "surrogateescape")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(encoded)))
        self.end_headers()
        return self.wfile.write(encoded)

try:
    HTTPServer(("0.0.0.0", 8000), BinDownloadHandler).serve_forever()
except KeyboardInterrupt:
    print("\nHTTP server stopped")
