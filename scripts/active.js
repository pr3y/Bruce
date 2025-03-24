const hamburgerBtn = document.getElementById("hamburger-btn");
const mobileNav = document.getElementById("mobile-nav");
const closeBtn = document.getElementById("close-btn");

hamburgerBtn.addEventListener("click", () => {
    mobileNav.classList.add("active");
});

closeBtn.addEventListener("click", () => {
    mobileNav.classList.remove("active");
});

document.querySelectorAll(".mobile-nav a").forEach(link => {
    link.addEventListener("click", () => {
        mobileNav.classList.remove("active");
    });
});

document.addEventListener("DOMContentLoaded", () => {

    const navLinks = document.querySelectorAll(".nav-link");
    let currentPage = window.location.pathname;

    const repoName = "/"; // rada rada rada
    if (currentPage.startsWith(repoName)) {
        currentPage = currentPage.replace(repoName, "");
    }

    if (currentPage === "" || currentPage === "/") {
        currentPage = "/index.html";
    }

    if (currentPage.endsWith('/')) {
        currentPage = currentPage.slice(0, -1);
    }

    navLinks.forEach(link => {
        let linkPath = link.getAttribute("href");

        if (linkPath.startsWith("http")) return;

        if (linkPath === "index.html" || linkPath === "/") {
            linkPath = "/index.html";
        }

        if (currentPage.includes(linkPath)) {
            link.classList.add("active");
        }
    });
});

