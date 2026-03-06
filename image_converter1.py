import base64
import os

def convert_image_to_base64():

    
    image_path = input("Enter the image filename or path: ").strip()
    image_path = image_path.strip('"').strip("'")
    
    script_directory = os.path.dirname(os.path.abspath(__file__))
    
    if not os.path.isabs(image_path) and not os.path.dirname(image_path):
        image_path = os.path.join(script_directory, image_path)
    
    if not os.path.exists(image_path):
        print()
        print("Error: File not found!")
        print("Please check the path and try again.")
        return
    
    if not os.path.isfile(image_path):
        print()
        print("Error: The path is not a file!")
        return
    
    print()
    print("Reading image file...")
    
    try:
        with open(image_path, 'rb') as image_file:
            image_binary_data = image_file.read()
    except Exception as e:
        print(f"Error reading file: {e}")
        return
    
    print("Converting to base64...")
    
    base64_encoded_data = base64.b64encode(image_binary_data)
    base64_string = base64_encoded_data.decode('utf-8')
    
    original_filename = os.path.basename(image_path)
    output_filename = os.path.join(script_directory, f"{original_filename}_base64.txt")
    
    print("Writing to text file...")
    
    try:
        with open(output_filename, 'w') as output_file:
            output_file.write(base64_string)
    except Exception as e:
        print(f"Error writing file: {e}")
        return
    

    print("The base64 text has been saved successfully!")
    print()

if __name__ == "__main__":
    convert_image_to_base64()