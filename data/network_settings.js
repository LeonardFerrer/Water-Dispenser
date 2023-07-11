function togglePasswordVisibility() {
    var passwordInput = document.getElementById("password");
    var showPasswordBtn = document.getElementById("showPasswordBtn");
    
    if (passwordInput.type === "password") {
        passwordInput.type = "text";
        showPasswordBtn.textContent = "...";
    } else {
        passwordInput.type = "password";
        showPasswordBtn.textContent = "...";
    }
}

function loadJSONData(jsonData) {
    var ssidInput = document.getElementById('ssid');
    var passwordInput = document.getElementById('password');
    var ipInput = document.getElementById('ip');
    var gatewayInput = document.getElementById('gateway');
    var subnetInput = document.getElementById('subnet');
  
    ssidInput.value = jsonData.ssid;
    passwordInput.value = jsonData.password;
    ipInput.value = jsonData.ip;
    gatewayInput.value = jsonData.gateway;
    subnetInput.value = jsonData.subnet;
  }
  
  function fetchData() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/network_settings.json', true);
    xhr.onreadystatechange = () => {
      if (xhr.readyState == 4 && xhr.status == 200) {
        var jsonData = JSON.parse(xhr.responseText);
        loadJSONData(jsonData);
      }
    };
    xhr.send();
  }
  
  window.addEventListener('load', fetchData);
    