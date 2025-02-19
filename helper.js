
function ompleLlistes() {
    escriuAccions("output/stocks_slope_percent_", "llista","csv")
    escriuAccions("output/stocks_sorted_stats_", "llista2","txt")
}

function escriuAccions(filenameSufix, elementId, ext) { // After the HTML content loads

    for (let i = 0; i < 10; i++) {
        let data = new Date();
        var dateOffset = (24*60*60*1000) * i;
        
        data.setTime(data.getTime() - dateOffset);
        var dia = formatejaData(data)
        
        dies = ["Diumenge", "Dilluns", "Dimarts", "Dimecres", "Dijous", "Divendres", "Dissabte"];
        // escriu els dies excepte per caps de setmana, ja que no hi ha canvis de borsa gairebé, els findes
        nom = dia + " " + dies[data.getDay()]
        filename = filenameSufix+dia + "."+ext
        if(data.getDay() != 6 && data.getDay() != 0) escriu_link(nom, filename, elementId);
      } 
    
};

function formatejaData(data) {
   
    const offset = data.getTimezoneOffset()
   
    data = new Date(data.getTime() - (offset*60*1000));
    
    var dataForm = data.toISOString().split('T')[0]

    

    return dataForm;
}

function escriu_link(nom,filename, elementId) { // After the HTML content loads
    let list = document.getElementById(elementId);
    var inner = "";

    inner += "<a href='"+filename +"'> accions dia " + nom + " </a>";
    
    let li = document.createElement('li');
    li.innerHTML =inner;
    list.appendChild(li); // Appends HTML to an element
};


const output = document.getElementById('main_stocks')


// document.getElementById('file').onchange = function() {
function carregaFitxer() {
    var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // Typical action to be performed when the document is ready:
          separaLinies(xhttp.responseText) //callback function
          escrDates(xhttp.responseText) //callback function
        }
    };
    xhttp.open("GET", "/output/pertiles.txt", true);
    xhttp.send();
};

function escrDates(text){
  var lines = text.split('\n');    // lines is an array of strings
  descrip = document.getElementById("descr")

  linia = lines[0].split(" ")
  linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
  data1 = linia[2]

  linia = lines[1].split(" ")
  linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
  data2 = linia[2]

  descrip.innerHTML = descrip.innerHTML +" "+ data1 + " --> " + data2;
}

linies = [] // variable global on estan totes les accions
cols = ["percentil actual", "creixement diari (del percentil)",
   "volatilitat", "guany esperat",
   "etiqueta de l'acció de borsa"] //variable global amb el nom de totes les columnes

function separaLinies(text) {
  var lines = text.split('\n');    // lines is an array of strings
  linies = []

  // Loop through all lines
  for (var j = 0; j < lines.length; j++) {
    linia = lines[j].split(' ')
    linia = linia.filter(item => item !== "") // deleting the stupid "" from the array
    if(!isNaN(linia[0])) {
      linies.push(linia)      
    }
  }
  
  //ordena array:
  linies.sort(ordenaMet1Array)
  EscriuTaula(linies,cols,"taula")
}

function ordenaCol(columna) {
  if(columna == 0) linies.sort(ordenaPerCol0)
  if(columna == 1) linies.sort(ordenaPerCol1)
  if(columna == 2) linies.sort(ordenaPerCol2)
  if(columna == 3) linies.sort(ordenaPerCol3)
  if(columna == 4) linies.sort(ordenaPerCol4)
  console.log("sorted out")

  EscriuTaula(linies,cols,"taula")
}

function EscriuTaula(array, nomCols, taulaId) {
  //nomCols = array string amb noms de les m columnes
  // array = matriu nxm amb n files 
  //  taualID: id de la taula on escriure tota la mandanga

  m = nomCols.length
  taula = document.getElementById(taulaId)
  taula.innerHTML = "";

  // insert header
    let row = taula.insertRow(-1); // Create an empty <tr> element and add it to the last position of the table:

      for(let i=0; i<m; ++i) {
        // Insert new cells (<td> elements) at the 1st and 2nd position of the "new" <tr> element:
        var cell = row.insertCell(i);

        // Add some text to the new cells:
        cell.innerHTML = "<span onclick='ordenaCol("+i+")'>"+nomCols[i]+ "</span>";
      }
    

  // Loop through all lines
  for (var j = 0; j < array.length; j++) {
      let row = taula.insertRow(-1); // Create an empty <tr> element and add it to the last position of the table:

      for(let i=0; i<m; ++i) {
        // Insert new cells (<td> elements) at the 1st and 2nd position of the "new" <tr> element:
        var cell = row.insertCell(i);

        // Add some text to the new cells:
        cell.innerHTML = array[j][i];
      }
  }
}


function ordenaMet1Array(arr1, arr2) {
  // ordenem l'array de la següent manera: la segona columna en blocs: 0, 0.1, 0.2, >0.3
  // un cop en blocs, s'ordena ascendentment segons la quarta columna
  if(Math.floor(parseFloat(arr1[1])*10)/10 == Math.floor(parseFloat(arr2[1])*10)/10) {
    return parseFloat(arr1[3])<parseFloat(arr2[3]);
  }
  return Math.floor(parseFloat(arr1[1])*10)/10 < Math.floor(parseFloat(arr2[1])*10)/10
}

function ordenaPerCol0(arr1, arr2) {
  return parseFloat(arr1[0]) > parseFloat(arr2[0])
}

function ordenaPerCol1(arr1, arr2) {
  return parseFloat(arr1[1]) < parseFloat(arr2[1])
}

function ordenaPerCol2(arr1, arr2) {
  a = arr1[2].split("%")[0]
  b = arr2[2].split("%")[0]
  return parseFloat(a) < parseFloat(b)
}

function ordenaPerCol3(arr1, arr2) {
  a = arr1[3].split("%")[0]
  b = arr2[3].split("%")[0]
  return parseFloat(a) < parseFloat(b)
}

function ordenaPerCol4(arr1, arr2) {
  return arr1[4] < arr2[4]
}