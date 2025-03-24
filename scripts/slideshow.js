document.addEventListener("DOMContentLoaded", () => {
    let currentSlide = 0;
    const slides = document.querySelectorAll('.slide');
    const dots = document.querySelectorAll('.dot');

    let autoSlideInterval = null;
    let autoSlideTimeout = null;

    function showSlide(index) {
        // Hide all slides and remove active class from dots
        slides.forEach(slide => slide.classList.remove('active'));
        dots.forEach(dot => dot.classList.remove('active'));

        // Show the selected slide and activate the corresponding dot
        currentSlide = (index + slides.length) % slides.length;
        slides[currentSlide].classList.add('active');
        dots[currentSlide].classList.add('active');
    }

    function goToSlide(index) {
        showSlide(index);
        resetAutoSlide();
    }

    function startAutoSlide() {
        if (autoSlideInterval) return;
        autoSlideInterval = setInterval(() => {
            showSlide(currentSlide + 1);
        }, 4000);
    }

    function resetAutoSlide() {
        clearInterval(autoSlideInterval);
        autoSlideInterval = null;
        clearTimeout(autoSlideTimeout);
        autoSlideTimeout = setTimeout(() => {
            startAutoSlide();
        }, 2000);
    }

    // Add click event listeners to dots
    dots.forEach((dot, index) => {
        dot.addEventListener('click', () => goToSlide(index));
    });

    showSlide(currentSlide);
    resetAutoSlide();
});

