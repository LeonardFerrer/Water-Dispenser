var gateway = `ws://${window.location.hostname}/ws`
var websocket;

function initWebsocket() {
    console.log('conectando...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('connection open');
    websocket.send("statusdispenser");
}

function onClose(event) {
    console.log('connection closed');
    setTimeout(initWebsocket, 3000);    // chama a função initWebSocket() após 3 segundos
}

function onMessage(event) {
    var data = event.data;
    console.log(data);
    if (data.slice(0, 7) == "onbotte") {
        let modal = document.getElementById("dv-onbotte");
        let modalTitulo = document.getElementById("onBotte-name");
        let modalCancelar = document.getElementById("onBotte-cancelar");
        let modalAguarde = document.getElementById("onBotte-aguarde");
        modal.style.display = "block";
        var botte = JSON.parse(data.slice(7));
        console.log(botte);

        if (botte["status"] == "off") {
            modal.style.display = "none";
            modalAguarde.style.display = 'none';
            modalCancelar.style.display = 'block';

        } else if (botte["status"] == "on") {
            modalAguarde.style.display = 'none';
            modalCancelar.style.display = 'block';

        } else if (botte["status"] == "busy") {
            modalAguarde.style.display = 'block';
            modalCancelar.style.display = 'none';
            modalTitulo.innerText = "DISPENSER EM USO!";

        } else if (botte["status"] == "continuous") {
            modalTitulo.innerText = "Continuo";
            modalCancelar.style.display = 'block';
        }
    } else if (data.slice(0, 21) == "completed_measurement") {
        var modalMedirVolume = document.getElementById("dv-modal-medir-volume");
        modalMedirVolume.classList.remove("show");
        var measure = JSON.parse(data.slice(21));
        inputVol = document.getElementById("inputVolume");
        inputVol.value = measure["volume"];
    }
}

window.addEventListener('load', onLoad);

function onLoad(event) {
    getBottes();
    initWebsocket();
    // createCardGarrafas("copo", "2.0");

}


// function createCardGarrafas(idCard, name, volume){
function createCardGarrafas(name, volume) {
    var card = document.createElement('div');
    card.classList.add("card");

    var botte = document.createElement('button');
    botte.id = "botte";
    botte.classList.add("card-botte");

    // mandando mensagem para o servidor
    botte.onclick = () => {
        // alert(name+" "+volume);
        onBotte(name, volume);
    }


    var nameBotte = document.createElement('div');
    // nameBotte.id = "name";
    nameBotte.classList.add("nameBotte");
    var text = document.createElement('h2');
    text.textContent = name;
    nameBotte.appendChild(text);

    botte.appendChild(nameBotte);

    var volumeBotte = document.createElement("div");
    // volumeBotte.id = "volume";
    volumeBotte.classList.add("volumeBotte");
    var text = document.createElement("h2");
    text.textContent = volume;
    volumeBotte.appendChild(text);

    var unid = document.createElement("h2");
    unid.textContent = "L";
    volumeBotte.appendChild(unid);

    botte.appendChild(volumeBotte);

    var buttonClose = document.createElement('button');
    buttonClose.classList.add("button-close");
    buttonClose.onclick = () => {
        if (card.parentNode) {
            card.parentNode.removeChild(card);
        }
    };

    buttonClose.id = "close";
    var text = document.createElement("h2");
    text.textContent = "X";
    buttonClose.appendChild(text);

    card.appendChild(botte);
    card.appendChild(buttonClose);

    var cards = document.querySelector(".cards");
    cards.appendChild(card);
}

function getBottes() {
    const request = new XMLHttpRequest();
    request.open("GET", "/bottes.json", true);
    request.send();

    request.onreadystatechange = () => {
        if (request.readyState == 4) {
            if (request.status == 200) {
                const json = JSON.parse(request.responseText);
                for (const i in json.bottes) {
                    let botte = json.bottes[i];
                    createCardGarrafas(botte.nome, botte.volume);
                }
            }
        }
    }
}

function editar() {
    let btnEdite = document.getElementById("editar");
    btnEdite.style.display = "none";

    let btnAdd = document.getElementById("btn-add");
    btnAdd.style.display = "block";

    // tornar visivel os botoes de excluir o botte
    let btns = document.querySelectorAll(".button-close");
    btns.forEach(btn => {
        btn.style.display = 'block';
    })

    let footer = document.getElementById("footer-form");
    footer.style.display = "flex";

    // Adicionar a função para editar os bottes individualmente
    editarBottes();
}

function addBotte() {
    let btnAdd = document.getElementById("dv-modal");
    btnAdd.style.display = 'block';
    document.getElementById("titulo-modal").innerText = "Nova Botte";
    document.getElementById("btn-salvar").innerText = "adicionar";
}

function onBotte(name, vol) {
    let modal = document.getElementById("dv-onbotte");
    let modalTitulo = document.getElementById("onBotte-name");
    var titulo = "";
    titulo = name + " " + vol + "L";
    botte = "onbotte" + JSON.stringify({ nome: name, volume: vol });
    console.log(botte);
    modalTitulo.innerText = titulo;
    websocket.send(botte);
    modal.style.display = 'block';
}

function onBotteContinuous() {
    let modal = document.getElementById("dv-onbotte");
    let modalTitulo = document.getElementById("onBotte-name");
    modalTitulo.innerText = "Continuo";
    websocket.send("onBotteContinuous");
    modal.style.display = 'block';
}

function offBotte() {
    let dispenser = document.getElementById("dv-onbotte");
    websocket.send("offbotte");
    dispenser.style.display = 'none';
}

function modalCancelar() {
    let btnAdd = document.getElementById("dv-modal");
    btnAdd.style.display = 'none';
}

function modalAdicionar() {
    var nome = document.getElementById("inputNome");
    var volume = document.getElementById("inputVolume");
    console.log(volume);
    if (nome.value != "" && volume.value != "") {
        createCardGarrafas(nome.value, volume.value);
        nome.value = "";
        volume.value = "";
        editar();
        modalCancelar();
    }
}


function cancelar() {
    window.location.href = "index.html";
}

function getBottesPage() {
    let json = {
        bottes: [],
        add: function (nome, vol) {
            botte = {
                nome: nome,
                volume: vol
            };
            this.bottes.push(botte);
        }
    };

    bottes = document.querySelectorAll("#botte");
    bottes.forEach(function (botte) {
        const name = botte.children[0].children[0].innerText;
        const volume = botte.children[1].children[0].innerText
        json.add(name, volume);
    });
    console.log(JSON.stringify(json));
    return (JSON.stringify(json));
}

function salvar() {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/updateBottes", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    bottes = getBottesPage();
    xhr.send(bottes);

    xhr.onreadystatechange = () => {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                location.reload();
                console.log("ok");
            } else {
                console.log("Error");
            }
        }
    }

}

function editarBottes() {
    let bottes = document.querySelectorAll("#botte");
    bottes.forEach(function (botte) {
        botte.onclick = () => {
            editarBotte(botte);
        };
    });
}

function editarBotte(botte) {
    let btnAdd = document.getElementById("dv-modal");
    document.getElementById("titulo-modal").innerText = "Editar Botte";
    btn = document.getElementById("btn-salvar");
    inputName = document.getElementById("inputNome");
    inputVol = document.getElementById("inputVolume");
    botteNome = botte.children[0].children[0];
    botteVol = botte.children[1].children[0];

    btnAdd.style.display = 'block';
    inputName.value = botteNome.innerText;
    inputVol.value = botteVol.innerText;
    btn.innerText = "salvar";

    btn.onclick = () => {
        if (inputName.value != "" && inputVol.value != "") {
            botteNome.innerText = inputName.value;
            botteVol.innerText = inputVol.value;
            inputName.value = "";
            inputVol.value = "";
            editar();
            modalCancelar();
        }
    }
}

function abrirModalMedirVolume() {
    var modalMedirVolume = document.getElementById("dv-modal-medir-volume");
    modalMedirVolume.classList.add("show");
    websocket.send("measured_volume_on");
    console.log("Abrindo modal measured_volume_on");
}

function concluirMedicao() {
    websocket.send("measured_volume_off");
    console.log("Medição de volume concluída measured_volume_off");
}


// function onBotte(name, vol){
//     const botte = new FormData();
//     botte.append('name', name);
//     botte.append('volume', vol);

//     const xhr = new XMLHttpRequest();
//     xhr.open("POST","/onBotte", true);
//     xhr.send(botte);

//     xhr.onreadystatechange = ()=>{
//         if(xhr.readyState == 4){
//             if(xhr.status == 200){
//                 console.log("ok");
//             } else {
//                 console.log("Error");
//             }
//         }
//     }
// }

