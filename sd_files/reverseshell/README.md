### **Reverse Shell with Web Interface**  

#### **How It Works**  

1. **Reverse Shell Communication**:  
   - Bruce connects to BruceC2 using TCP.
   - Commands sent from Bruce are executed on the Machine. (No output.)

2. **Web Interface**:  
   - Bruce hosts a web server on its local IP (`192.168.4.1`).
   - The `/` endpoint provides a basic interface to check the connection status and interact with the shell.

3. **Command Usage in BruceC2**:  
   BruceC2 enables advanced command handling. Some examples include:  
   - `exit`: Disconnect the ESP32 and terminate the session.  
   - `clear`: Clear the terminal on the server side.  
   - `B:<command>`: Execute a **Bash command** on the server (e.g., `B:ls`).  
   - `PS:<command>`: Execute a **PowerShell command** on the server (e.g., `PS:Get-Process`).  
   - `showConsole`: Show the console output in BruceC2.  
   - `hideConsole`: Hide the console output in BruceC2.  

---

#### **Features**  

- **Reverse Shell Communication**:  
   Execute and receive commands via BruceC2 remotely.  

- **Web-Based Monitoring**:  
   Access the Reverse shell status and execution through a simple web interface.  

- **Cross-Platform Command Execution (Not really fully supported yet.)**:  
   Use platform-specific commands (Bash or PowerShell) through BruceC2.

- **Automatic Connecting to Bruce AP**:  
   BruceC2 automatic searches for BruceC2 and instantly connects to it.

---

#### **Usage Instructions**  

1. **Set Up Bruce**:
   - Start "**Reverse Shell Mode**" in Bruce.  
   - Start and Wait for the BruceC2 to connect.

2. **Access Web Interface**:  
   - Navigate to the web server (`http://192.168.4.1`) using a browser.
   - Use the interface to monitor the connection status or execute shell commands.

#### **Optimal Execution with BadUSB Scripts**  

The best way to execute BruceC2 is by utilizing **BadUSB scripts**. This approach automates the setup and ensures quick deployment of the reverse shell server:  

1. **Prepare the Payload**:  
   Create a BadUSB script to start BruceC2 on the target machine automatically.  

2. **Execute**:  
   Insert the BadUSB device into the target computer. The script will launch BruceC2 and establish a reverse shell connection.  

3. **Benefits**:  
   - Instant setup with minimal manual intervention.  
   - Ideal for penetration testing and red team operations.