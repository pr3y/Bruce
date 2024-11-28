### **Setting AP Name from HTML**  

#### **Overview**  
The `EvilPortal` system now supports the ability to define an Access Point (AP) name directly within your HTML files. By including a specific tag in the first line of your HTML file, the system will automatically extract and set the AP name, streamlining the setup process.

---

#### **How It Works**  
1. Add the following tag in the **first line** of your HTML file:  
   ```html
   <!-- AP="YourCustomAPName" -->
   ```
   Replace `YourCustomAPName` with the desired name for your Access Point.  

2. When the HTML file is loaded, the system will:  
   - Parse the first line of the file.  
   - Detect the `AP="..."` tag.  
   - Extract the value and set it as the AP name.  

3. If the tag is not present it will ask you for AP name (as usual).

---

#### **Example HTML File**  
```html
<!-- AP="MyCoolNetwork" -->
<!DOCTYPE html>
<html>
<head>
    <title>EvilPortal</title>
</head>
<body>
    <h1>Welcome to EvilPortal!</h1>
</body>
</html>
```

- In this example, the AP name will automatically be set to **MyCoolNetwork**.

---

#### **Benefits**  
- **Dynamic Configuration**: Easily customize AP names without modifying code.  
- **Ease of Use**: Set up AP names directly in your HTML files for faster deployment.  

---

#### **Notes**  
- Ensure the `<!-- AP="..." -->` tag is in the **very first line** of the file.  
- The feature does not affect the functionality of other HTML content.