<script>
    import ColorPicker from '$lib/ColorPicker.svelte';
  
    let width = 5;
    let height = 5;
    let colors = [];

    // Reactive statement to adjust the grid size based on width and height
    $: {
        colors = Array.from({ length: height }, () =>
        Array.from({ length: width }, () => '#ffffff')
        );
    }

    function handleColorChange(x, y, event) {
      colors[x][y] = event.detail.color;
    }
  
    function exportBitmap() {
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
      a.download = 'bitmap.BMP';
      a.click();
    });
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
      display: grid;
      grid-template-columns: repeat(var(--width), 50px); /* dynamic number of columns based on width */
      grid-gap: 10px;
      justify-content: center; /* center the grid horizontally */
    }
  
    input[type="number"], button {
      margin: 0.5em;
    }
  </style>
  
  <div class="container">
    <div>
      <label for="width">Width:</label>
      <input id="width" type="number" bind:value={width} min="1" max="32">
      <label for="height">Height:</label>
      <input id="height" type="number" bind:value={height} min="1" max="32">
      <button on:click={exportBitmap}>Export Bitmap</button>
    </div>
    <div class="grid" style="--width: {width};">
      {#each colors as row, x}
        {#each row as color, y}
          <ColorPicker bind:color={colors[x][y]} on:change={(e) => handleColorChange(x, y, e)} />
        {/each}
      {/each}
    </div>
  </div>