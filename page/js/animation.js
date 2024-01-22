const toogleBtn = document.querySelector('.toogleBtn');

toogleBtn.addEventListener('click', () => {
});

const loginForm = document.querySelector('form');
const onSubmit = function(event) {
    event.preventDefault();
}

var pw = document.getElementById('password');
var c_pw = document.getElementById('confirm_password');

function reconfirmPassword() {
    if(pw.value != c_pw.value) {
        c_pw.setCustomValidity("비밀번호가 일치하지 않습니다.");
        loginForm.addEventListener("submit", onSubmit);
    } else {
        c_pw.setCustomValidity('');
    }
}

// pw.onchange = reconfirmPassword();
// c_pw.onchange = reconfirmPassword();