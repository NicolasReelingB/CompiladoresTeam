<script lang="ts">

    import ColorPalette from "$lib/ColorPalette.svelte";
    import AnimatedBackground from "$lib/AnimatedBackground.svelte";
    import { invoke } from '@tauri-apps/api/tauri';
    import { read } from "$app/server";

    let width = 5;
    let height = 5;
    let colors = [];
    let hexColors = [];
    let outputResponse = '';

    // Reactive statement to adjust the grid size based on width and height
    $: {
        colors = Array.from({ length: height }, () =>
        Array.from({ length: width }, () => '#ffffff')
        );
    }

    function handleColorChange(x, y, event) {
      colors[x][y] = event.detail.color;
    }

    function removeHashes(matrix) {
      return matrix.map(row => row.map(color => color.replace('#', '').toUpperCase()));
    }

    async function exportBitmap() {
      const canvas = document.createElement('canvas');
      canvas.width = width * 50;
      canvas.height = height * 50;
      const ctx = canvas.getContext('2d');

      colors.forEach((row, x) => {
        row.forEach((color, y) => {
          ctx.fillStyle = color;
          ctx.fillRect(y * 50, x * 50, 50, 50);
        });
      });

      canvas.toBlob((blob) => {
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'main.bmp';
        a.click();
      });

    let processedColors = removeHashes(colors);
    console.log(processedColors)
    try {
      const response = await invoke('process_matrix', { matrix: processedColors });
      console.log('Response from Rust:', response);
      outputResponse = response;
    } catch (error) {
      console.error('Error processing matrix:', error);
    }
  }

  function handleFileChange(event) {
        const file = event.target.files[0];
        if (file && file.type === 'image/bmp') {
            readBitmap(file);
        } else {
            alert('Please upload a valid bitmap (.bmp) file.');
        }
    }

  async function readBitmap(file) {
        const reader = new FileReader();

        reader.onload = (e) => {
            const img = new Image();
            img.src = e.target.result;

            img.onload = () => {
                const canvas = document.createElement('canvas');
                const ctx = canvas.getContext('2d');

                canvas.width = img.width;
                canvas.height = img.height;
                ctx.drawImage(img, 0, 0);

                const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                const data = imageData.data;

                hexColors = [];
                for (let i = 0; i < data.length; i += 4) {
                    const r = data[i];
                    const g = data[i + 1];
                    const b = data[i + 2];
                    const hex = rgbToHex(r, g, b);
                    hexColors.push(hex);
                }
                console.log(hexColors);
            };
        };

        reader.readAsDataURL(file);

        try {
          const response = await invoke('process_matrix', { matrix: hexColors });
          console.log('Response from Rust:', response);
        } catch (error) {
          console.error('Error processing matrix:', error);
        }
    }

    function rgbToHex(r, g, b) {
        return ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1).toUpperCase();
    }


  let filePath = '../../compiler/main.cpp';
  let outputPath = '../../compiler/main';
  let output = '';

  async function compileAndRunCpp() {
    try {
      output = await invoke('compile_and_run_cpp', {filePath, outputPath});
      console.log('Program output:', output);
    } catch (error) {
      console.error('Error:', error);
      output = 'An error occurred: ' + error;
    }
  }



  async function downloadCppFile(fileName) {
    try {
      const fileContents = await invoke('get_generated_cpp_file', { fileName });
      const blob = new Blob([new Uint8Array(fileContents)], { type: 'text/x-c++src' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = fileName;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    } catch (error) {
      console.error('Error downloading file:', error);
    }
  }
</script>

  <style>

    .container {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh; /* Adjust height to take full viewport height */
    }

    .grid {
      margin-top: 1em;
      display: grid;
      grid-template-columns: repeat(var(--width), 28px); /* dynamic number of columns based on width */
      grid-gap: 0;
      justify-content: center; /* center the grid horizontally */
      background-color: #8f8f8f;

    }

    input[type="range"], button {
      margin-left: 2em;

    }
    input[type="file"], button{
      font-family: "Bauhaus 93";
      padding: 5px;
    }
    label{
      background-color: #cecece;
      padding: 15px ;
      border-radius: 20px;

    }
    textarea{
      border-radius:  10px;
    }
    button{
      border-radius: 10px;
      padding: 5px;
      font-family: "Bauhaus 93";
    }
    :root{
      font-family: "Bauhaus 93"!important;
    }

  </style>
<AnimatedBackground/>

  <div class="container">
    <div>
      <label for="width">Width:</label>
      <input id="width" type="range" bind:value={width} min="1" max="32">
      <label for="height">Height:</label>
      <input id="height" type="range" bind:value={height} min="1" max="16">
      <button on:click={exportBitmap} on:click={() => downloadCppFile('main.cpp')}>Export Bitmap</button>
      <input type="file" accept=".bmp" on:change={handleFileChange}>
      <button on:click={compileAndRunCpp}>Compile cpp</button>
    </div>
    <div class="grid" style="--width: {width};">
      {#each colors as row, x}
        {#each row as color, y}
          <ColorPalette bind:color={colors[x][y]} on:click={(e) => handleColorChange(x, y, e)} />
        {/each}
      {/each}
    </div>
    <textarea bind:value={outputResponse} readonly/>
  </div>
