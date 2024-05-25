document.addEventListener('DOMContentLoaded', function () {
    const form = document.getElementById('check-form');
    const result = document.getElementById('result');
    const backButton = document.getElementById('back-button');

    if (form) {
        form.addEventListener('submit', async function (event) {
            event.preventDefault();
            const username = document.getElementById('username').value;
            const response = await fetch('/check-privilege', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ username })
            });
            const data = await response.json();
            localStorage.setItem('username', data.username);
            localStorage.setItem('privilege', data.privilege);
            window.location.href = 'result.html';
        });
    }

    if (result) {
        const username = localStorage.getItem('username');
        const privilege = localStorage.getItem('privilege');
        result.textContent = privilege;
        backButton.addEventListener('click', function () {
            window.location.href = 'index.html';
        });
    }
});
