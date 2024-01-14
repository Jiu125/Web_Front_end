const toogleBtn = document.querySelector('.navbar_toogleBtn');
const meau = document.querySelector('.navbar_meau');
const search = document.querySelector('.navbar_search');

toogleBtn.addEventListener('click', () => {
    meau.classList.toggle('active');
    search.classList.toggle('active');
});