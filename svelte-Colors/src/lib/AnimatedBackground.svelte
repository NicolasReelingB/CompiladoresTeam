<style>
    .bg {
        animation:slide 3s ease-in-out infinite alternate;
        background-image: linear-gradient(30deg, #e38a1d 30%, #1a796e 40%, #791a53 30%);
        bottom:0;
        left:-50%;
        opacity:.5;
        position:fixed;
        right:-50%;
        top:0;
        z-index:-1;
    }

    .bg2 {
        animation-direction:alternate-reverse;
        animation-duration:4s;
    }

    .bg3 {
        animation-duration:5s;
    }

    @keyframes slide {
        0% {
            transform:translateX(-25%);
        }
        100% {
            transform:translateX(25%);
        }
    }
</style>


<div class="bg"></div>
<div class="bg bg2"></div>
<div class="bg bg3"></div>