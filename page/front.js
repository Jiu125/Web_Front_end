function register(evt) {
    evt.preventDefault();
    const id = evt.target.id.value;
    const pw = evt.target.password.value;
    if (!id || !pw) {
      return alert('모두 입력하세요');
    }
  
    const url = 'URL';
    const data = { id, pw };
    fetch('URL', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }
  
  document.querySelector('form').addEventListener('submit', registerFood);