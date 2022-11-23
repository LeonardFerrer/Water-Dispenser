function onload(){
    getBottes();
}

function createCardGarrafas(idCard, titulo, volume){
    var pai = document.querySelector(".column");
    var card = document.createElement('button');
    card.id = idCard;
    card.onclick = function(){alert("oi")};
    card.classList.add("card");

    var divTitulo = document.createElement('div');
    divTitulo.id = "titulo";
    divTitulo.classList.add("nome");
    var tituloCard = document.createElement('h2');
    tituloCard.textContent = titulo;
    divTitulo.appendChild(tituloCard);
    card.appendChild(divTitulo);

    var DivVolume = document.createElement("div");
    DivVolume.id = "volume";
    DivVolume.classList.add("garrafa-volume");
    var text = document.createElement("h2");
    text.textContent = volume;
    DivVolume.appendChild(text);
    card.appendChild(DivVolume);

    pai.appendChild(card);
}

function getBottes(){
    const request = new XMLHttpRequest();
    request.open("GET", "/bottes.json", true);
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    request.send();

    request.onreadystatechange = ()=>{
        if(request.readyState == 4){
            if(request.status == 200){
                const json = JSON.parse(request.responseText);
                console.log(json);
                for (const i in json.bottes){
                    let botte = json.bottes[i];
                    createCardGarrafas(i, botte.nome, botte.volume);
                }
            }
        }
    }
    
}

let modal = document.querySelector('.Modal-container');
function openModal(){   modal.classList.add('active');}
function closeModal(){  modal.classList.remove('active');}
