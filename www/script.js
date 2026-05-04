const output = document.getElementById("output");

function show(res) {
    output.textContent = res;
}

function testGet() {
    fetch("/")
        .then(r => r.text())
        .then(show)
        .catch(e => show(e));
}

function testFile() {
    fetch("/files/test.txt")
        .then(r => r.text())
        .then(show);
}

function testPost() {
    const data = document.getElementById("postData").value;

    fetch("/upload", {
        method: "POST",
        body: data
    })
    .then(r => r.text())
    .then(show);
}

function testDelete() {
    fetch("/files/test.txt", {
        method: "DELETE"
    })
    .then(r => r.text())
    .then(show);
}

function runCgi() {
    fetch("/cgi-bin/hello.py")
        .then(r => r.text())
        .then(show);
}

function showCookies() {
    show("Cookies:\n" + document.cookie);
}

function setCookie() {
    document.cookie = "theme=dark";
    show("Cookie set from JS");
}

function setCustomCookie() {
    const key = document.getElementById("cookieKey").value;
    const value = document.getElementById("cookieValue").value;

    document.cookie = key + "=" + value + "; path=/";

    show("Cookie set: " + key + "=" + value);
}

function showCookies() {
    show("Cookies:\n" + document.cookie);
}

