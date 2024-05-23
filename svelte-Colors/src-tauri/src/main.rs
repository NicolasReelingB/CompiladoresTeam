use tauri::{Builder, generate_context, generate_handler};
use std::process::Command;
use std::path::Path;
use std::fs::{self, File};
use std::io::Read;
use std::io::Write;


#[tauri::command]
fn compile_and_run_cpp(file_path: String, output_path: String) -> Result<String, String> {
    if !Path::new(&file_path).exists() {
        return Err(format!("File not found: {}", file_path));
    }

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
    println!("Received file name: {}", file_name);
    let file_path = format!("../../compiler/output/{}", file_name);
    println!("Constructed file path: {}", file_path);
    if !std::path::Path::new(&file_path).exists() {
        println!("File not found: {}", file_path);
        return Err(format!("File not found: {}", file_path));
    }
    let mut file = File::open(file_path).map_err(|e| e.to_string())?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer).map_err(|e| e.to_string())?;
    Ok(buffer)
}

#[tauri::command]
fn process_matrix(matrix: Vec<Vec<String>>) -> Result<String, String> {
    // Debug print to verify the matrix is received correctly
    println!("Received matrix: {:?}", matrix);

    // Serialize the matrix into a format that your C++ code can understand
    let matrix_str = matrix.iter()
        .map(|row| row.join(" "))
        .collect::<Vec<_>>()
        .join("\n");

    // Write the matrix to a temporary file (or pass it directly)
    let matrix_file_path = "matrix.txt";
    let mut file = File::create(matrix_file_path).map_err(|e| e.to_string())?;
    file.write_all(matrix_str.as_bytes()).map_err(|e| e.to_string())?;

    // Compile and run the C++ program with the matrix file as input
    let output = Command::new("g++")
        .args(&["-o", "compiled_file", "../../compiler/main.cpp"])
        .output()
        .map_err(|e| e.to_string())?;

    if !output.status.success() {
        return Err(String::from_utf8_lossy(&output.stderr).to_string());
    }

    let output = Command::new("./compiled_file")
        .arg(matrix_file_path)
        .output()
        .map_err(|e| e.to_string())?;

    if !output.status.success() {
        return Err(String::from_utf8_lossy(&output.stderr).to_string());
    }

    let result = String::from_utf8_lossy(&output.stdout).to_string();
    fs::remove_file(matrix_file_path).map_err(|e| e.to_string())?;
    fs::remove_file("compiled_file.exe").map_err(|e| e.to_string())?;
    Ok(result)
}

fn main() {
    Builder::default()
        .invoke_handler(generate_handler![get_generated_cpp_file, compile_and_run_cpp, process_matrix])
        .run(generate_context!())
        .expect("error while running tauri application");
}