<script>
    import { createEventDispatcher } from 'svelte';

    function colorCallback(rgba) {
      console.log(rgba.detail)
    }
    export let color = '#ffffff'; // default color
    const dispatch = createEventDispatcher();
  
    function updateColor(event) {

      color = event.target.value;
      dispatch('change', { color });

    }
  </script>
  <style>

    .hidden{
      height: 24px;
      width: 24px;
      border: none;
      display: block;
      padding: 0;

    }
    .hidden::-webkit-color-swatch-wrapper {
       padding: 0;
     }
  </style>
 <svelte:options accessors />

  <input type="color" value={color} on:change={updateColor} class="hidden">