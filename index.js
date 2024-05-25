const express = require('express');
const path = require('path');
const opn = require('opn');
const addon = require('./native/build/Release/addon');

const app = express();
const port = 3000;

app.use(express.static(path.join(__dirname, 'public')));
app.use(express.json());

app.post('/check-privilege', (req, res) => {
    const username = req.body.username;
    const privilege = addon.getUserPrivilege(username);
    if (privilege === "") {
        res.json({ username, privilege: `Пользователя ${username} нет` });
    } else {
        res.json({ username, privilege: `${privilege}` });
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
    opn(`http://localhost:${port}`);
});
