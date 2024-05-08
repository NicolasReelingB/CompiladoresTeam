<script>
    import { createEventDispatcher } from 'svelte';
  
    export let color = '#ffffff'; // default color
    const dispatch = createEventDispatcher();
  
    function updateColor(event) {
      color = event.target.value;
      dispatch('change', { color });
    }
  </script>
  
  
  <input type="color" value={color} on:change={updateColor} class="hidden">