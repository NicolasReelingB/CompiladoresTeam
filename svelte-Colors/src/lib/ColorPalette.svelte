<script>
    // v1.2 - added keyboard nav for colors
    // @todo still a bug when the dropdown opens, we should focus on the already selected color, this only works when you click it open, close it and open again


    import { clickOutside } from '$lib/utils/clickOutside.js';
    import { tick, onMount } from 'svelte';
    import { createEventDispatcher } from 'svelte';
    import ColorPicker from "$lib/ColorPicker.svelte";

    const dispatch = createEventDispatcher();
    // Initial value
    export let id = "ioawfoeskoa";
    export let color = '#ffffff';

    // Our color set
    let values = [
        ['#FFAADD', '#FFAAAA', '#FFAA77', '#FFAA44', '#FF8811', '#FF5511', '#000033'],
        ['#0000FE', '#0000EE', '#0000DD', '#0000CC', '#0000BB', '#0000AA', '#000099'],
        ['#000088', '#000077', '#000066', '#2BB2FF', '#FF7B2B', '#AA1111', '#AA5555'],
        ['#AAFFFF', '#AACCCC', '#CC33FF', '#FFFF55', '#FFD0D0', '#50C878', '#F62217'],
        ['#33FF77', '#33FF44', '#33FFDD', '#33FFAA', '#4AEEB3', '#40CCB0', '#FF5353'],
        ['#FF0000', '#0000FF', '#00FF00', '#00FFFF', '#FFFF00', '#FF00FF', '#BC7FFF'],
        ['#ABCD0E', '#2024AB', '#6A09B0', '#FFFFFF', '#000000', '#FFD37C']
    ]
    let tokens = [
        ['+', '-', '*', '/', '%', '=', '.'],
        ['0', '1', '2', '3', '4', '5', '6'],
        ['7', '8', '9', '(', ')', '<', '>'],
        ['==', '!=', '&&', '||', '!', 'T', 'F'],
        ['if', 'else', 'for', 'wh', '~', '|', ';'],
        ['ch', 'num', 'bool', 'arr', 'str', 'fun', 're'],
        ['_', 'out', 'read', ' ', '$', '\"']
    ]

    // Keyboard shortcut
    let trigger = 'Escape';
    function handleKeydown(e) {
        if (e.key == trigger) {
            ddActive = false;
        }
    }

    let windowHeight;
    let top;

    let ddActive = false;

    let ddHeight = 158;
    // ddHeight is initially undefined so we can't get the correct values from binding; that's why we have a default
    // todo render offscreen for .1sec to get the height automatically?

    let inputHeight;

    async function toggleDropdown(e) {
        if (
            (e.clientY + inputHeight) < ddHeight
            ||
            (windowHeight - ddHeight - inputHeight - e.clientY) > 0
        ) {
            top = false;
        } else {
            top = true;
        }

        ddActive = !ddActive

        await tick();
        if (ddActive) {
            //document.querySelector('.color-block.active').focus();
        }
    }


    function clickOutsideDropdown() {
        ddActive = false;
    }

    function changeValue(innerValue) {

        color =innerValue;
        ddActive = false;
        dispatch('change', { color });

    }
    let csColor;
    function  customColor(){
        color= csColor.color
        ddActive = false;
        dispatch('change', { color });
    }

    function keyboardGridNav(e, index) {


        const focussedElement = document.activeElement.id;

        let myRow = parseInt(focussedElement.charAt(focussedElement.length-3));
        let myIndex = parseInt(focussedElement.charAt(focussedElement.length-1));
        let nextRow;
        let prevRow;
        let nextIndex;
        let prevIndex;

        switch(e.keyCode) {
            // left arrow
            case 37:
                prevIndex = myIndex-1;
                if (prevIndex > -1) {
                    document.getElementById(id+'-'+myRow+'-'+prevIndex).focus();
                }
                break;
            // top arrow
            case 38:
                prevRow = myRow-1;
                if (prevRow > -1) {
                    document.getElementById(id+'-'+prevRow+'-'+myIndex).focus();
                }
                break;
            // right arrow
            case 39:
                nextIndex = myIndex+1;
                if ( nextIndex < values[0].length) {
                    document.getElementById(id+'-'+myRow+'-'+nextIndex).focus();
                }
                break;
            // down arrow
            case 40:
                nextRow = myRow+1;
                if (nextRow < values.length) {
                    document.getElementById(id+'-'+nextRow+'-'+myIndex).focus();
                }
                break;
        }

    }




</script>

<svelte:window bind:innerHeight={windowHeight} on:keydown={handleKeydown} />

<div class="color-picker-holder">

        <button bind:clientHeight={inputHeight} class="select-color"   on:click={(e) => toggleDropdown(e)} class:fake-focus={ddActive}>

                <div  style="background: {color};" class="color-block"></div>

        </button>


    {#if ddActive}
        <div class:top={top} bind:clientHeight={ddHeight} class="values-dropdown" use:clickOutside on:click_outside={clickOutsideDropdown}>
            <div class="values-dropdown-grid">
                {#each values as val, index}
                    {#each val as innerValue, innerIndex}
                        <div class="values-container">


                        <button
                                id="{id}-{index}-{innerIndex}"
                                class:active={innerValue == color}
                                on:keydown={(e) => keyboardGridNav(e,  innerIndex)}
                                style="background: {innerValue};"
                                on:click={() => { changeValue(innerValue)}}
                                class="color-block">
                        </button>

                            <label class="overlay-text">{tokens.at(index).at(innerIndex)}</label>

                        </div>


                    {/each}

                {/each}
                <ColorPicker bind:this={csColor} on:change={()=> {customColor()}}/>
            </div>
        </div>
    {/if}
</div>


<style>

    .color-picker-holder {
        position: relative;
    }

    .select-color{
        padding: 0;
    }




    .active {
        box-shadow: inset 0 0 0 1px #FFF, 0 0 3px 1px rgba(0,0,0,0.25);
    }

    .fake-focus, input:focus, button:focus  {
        outline: 0;
        box-shadow: 0 0 0 2px #18A0FB;
        border-color: #18A0FB;
    }

    input {

        height: 24px;

    }

    .color-block {
         width: 24px; height: 24px; line-height: 0; font-size: 0;
    }

    .values-dropdown {
        padding: 1rem;
        position: absolute;
        z-index: 1;
        top: 40px;
        background: white;
        border: 1px solid #CCC;
        border-radius: .3rem;
    }

    .values-dropdown-grid {
        grid-template-columns: repeat(7, 24px);
        grid-template-rows: 24px 24px;
        grid-gap: 10px;
        display: grid;

    }

    .values-dropdown.top {
        top: auto;
        bottom: 40px;
    }

    .values-dropdown button {
        border: none;
    }
    .values-container {
        position: relative;
        text-align: center;

    }
    .overlay-text {
        position: absolute;
        top: 60%;
        left: 50%;
        transform: translate(-50%, -50%);
        font-weight: normal;
        font-size: 0.8em;
        font-family: "Bauhaus 93";
        color: #FFFFFF;
        -webkit-user-select: none;
        -ms-user-select: none;
        user-select: none;
        pointer-events: none;
    }

</style>