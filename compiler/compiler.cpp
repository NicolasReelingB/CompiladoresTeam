#include "to_code/compiler_to_code.hpp"
#include "to_bmp/compiler_to_bmp.hpp"

int main() {
    CompilerToCode compilerToCode;
    compilerToCode.bitmapToCode("to_code/input/input1.txt");

    CompilerToBmp compilerToBmp;
    compilerToBmp.codeToBitmap("to_bmp/input/input1.bmp");
    
    return 0;
}