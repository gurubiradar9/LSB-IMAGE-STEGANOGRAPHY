# 🖼️ LSB Image Steganography using C (with Magic String)

## 📘 Overview
This project demonstrates **Image Steganography** using the **Least Significant Bit (LSB)** technique implemented in **C programming language**.  
Steganography is a method of hiding secret data within an image so that it remains invisible to the human eye.  

In this project, a **magic string** is used as a marker to identify the presence of hidden data inside the image.  
For demonstration, we hide a **dummy PIN** or secret text inside a `.bmp` image file.

---

## 🧠 Concept

- **Steganography:** Technique of hiding secret data (like text or files) inside an image.
- **LSB (Least Significant Bit):** The last bit of each pixel byte is modified to store hidden information.
- **Magic String:** A unique identifier (e.g., `"#*MAGIC*#"`) used to mark the start of secret data, helping the decoder know where to begin extraction.

---

## 🛠️ Features
- Developed entirely in **C language**.
- Supports **.bmp** image format.
- Embeds and extracts secret data safely.
- Uses a **magic string** to detect hidden content.
- Maintains the same appearance as the original image.

---

## ⚙️ Requirements
- GCC compiler (or any C compiler)
- Windows/Linux operating system
- `.bmp` image for testing
- Secret file (like `.txt`, `.csv`, `.py`)

---

## 💻 How to Run

### **1️⃣ Compilation**
```bash
gcc main.c encode.c decode.c common.c -o stego  or gcc *.c
## for encoding
./a.out -e <original.bmp image> <secret.txt><output.bmp[optional]>
##for decoding
./a.out -d <output.bmp image> <output[optional]file>

