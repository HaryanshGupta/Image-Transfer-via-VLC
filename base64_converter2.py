import base64
import os

def convert_base64_to_png():

    # Ask user for the base64 string
    print("Enter or paste your base64 string:")
    base64_string = input().strip()
    
    # Check if the string is empty
    if not base64_string:
        print()
        print("Error: No base64 string provided!")
        return
    
    # Get the directory where this script is located
    script_directory = os.path.dirname(os.path.abspath(__file__))
    
    
    print()
    print("Decoding base64...")
    
    # Decode base64 string to binary data
    try:
        image_binary_data = base64.b64decode(base64_string)
    except Exception as e:
        print(f"Error decoding base64: {e}")
        print("Make sure you pasted valid base64 data.")
        return
    
    # Ask user for output filename
    print()
    output_name = input("Enter output filename (e.g., myimage.png): ").strip()
    
    # If user didn't provide extension, add .png
    if not output_name.lower().endswith('.png'):
        output_name = output_name + '.png'
    
    # Create full output path
    output_filename = os.path.join(script_directory, output_name)
    
    print("Writing PNG file...")
    
    # Write binary data to PNG file
    try:
        with open(output_filename, 'wb') as output_file:
            output_file.write(image_binary_data)
    except Exception as e:
        print(f"Error writing file: {e}")
        return
    
   

# Run the function
if __name__ == "__main__":
    convert_base64_to_png()
