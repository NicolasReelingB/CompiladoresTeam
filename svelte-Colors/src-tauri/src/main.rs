use tauri::{Builder, generate_context, generate_handler};
use std::process::Command;
use std::path::Path;
use std::fs::File;
use std::io::Read;

#[tauri::command]
fn compile_and_run_cpp(file_path: String, output_path: String) -> Result<String, String> {
    // Ensure the input file exists
    if !Path::new(&file_path).exists() {
        return Err(format!("File not found: {}", file_path));
    }

    // Compile the C++ file
    let compile_output = Command::new("g++")
        .arg(&file_path)
        .arg("-o")
        .arg(&output_path)
        .output()
        .expect("Failed to execute g++");

    if !compile_output.status.success() {
        return Err(format!(
            "Compilation failed: {}",
            String::from_utf8_lossy(&compile_output.stderr)
        ));
    }

    // Execute the compiled binary
    let run_output = Command::new(&output_path)
        .output()
        .expect("Failed to execute the compiled program");

    if run_output.status.success() {
        Ok(String::from_utf8_lossy(&run_output.stdout).to_string())
    } else {
        Err(format!(
            "Execution failed: {}",
            String::from_utf8_lossy(&run_output.stderr)
        ))
    }
}

#[tauri::command]
fn get_generated_cpp_file(file_name: String) -> Result<Vec<u8>, String> {
    println!("Received file name: {}", file_name); // Debug print
    let file_path = format!("../testfiles/{}", file_name);
    println!("Constructed file path: {}", file_path); // Debug print
    if !std::path::Path::new(&file_path).exists() {
        println!("File not found: {}", file_path); // Debug print
        return Err(format!("File not found: {}", file_path));
    }
    let mut file = File::open(file_path).map_err(|e| e.to_string())?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer).map_err(|e| e.to_string())?;
    Ok(buffer)
}

fn main() {
    Builder::default()
        .invoke_handler(generate_handler![get_generated_cpp_file, compile_and_run_cpp])
        .run(generate_context!())
        .expect("error while running tauri application");
}