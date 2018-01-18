/*
 *
 * bestreads.js - javascript to request data from 
 *                bestreads.php and insert data into
 *								bestreads.html
 *
 * Account: csi345_07
 * File: ~/www/project/bestreads.js
 * Honor Code: We pledge the AIC: Joe Gasper
 *
 */

// Called when window is loaded, puts in javascript strict mode
"use strict";
window.onload = pageLoad;

// variables declared to use in strict mode
var xhr;
var x1;
var x2;
var jpar;
var home = null;
var newDiv = null;
var newImg = null;
var newPar = null;
var singleBook = null;
var folderList = null;
var i = 0;

//
// pageLoad() - function to call showAllBooks upon page loading.
//              Also has back button to return to home page.
//
function pageLoad() {

    // Retrieve data from server
    xhr = new XMLHttpRequest();
		xhr.onreadystatechange = showAllBooks;
		
		//If not on home page do this..., otherwise books keep loading into allbooks
		if(document.getElementById("allbooks").style.visibility != "visible"){
		    xhr.open("GET","bestreads.php?mode=books", false);
		    xhr.send(null);
		}		
		// go back to home page via back button
		home = document.getElementById('back');
		home.setAttribute('onclick','pageLoad()');
}
//
//
// showAllBooks()- Function to display all books available.
//                 Will display the book's cover and title.
//                 When book is clicked, single book data will be shown.
//
function showAllBooks() {
    if(xhr.readyState == 4 && xhr.status == 200) {
		    // Get response from server
				var parser = new DOMParser();
			 	var xmlDoc = parser.parseFromString(xhr.responseText, "application/xml");
				var allbooks = document.getElementById("allbooks");
				var bookList = xmlDoc.getElementsByTagName("book");
				    
				// retreive all book titles and directories
				for(i=0; i<bookList.length; i++) {
						var title = bookList[i].firstChild.innerHTML;
						var dir = bookList[i].lastChild.innerHTML;
						
						// create new img element 
						newImg = document.createElement("img");
						newImg.setAttribute("src", "books/" + dir + "/cover.jpg");
						newImg.setAttribute("alt",dir);
						// create new div element
						newDiv = document.createElement("div");
						newDiv.setAttribute("id", dir);
						newDiv.setAttribute('onclick','loadBook(\''+dir+'\')');
						
						//create new p element
						newPar = document.createElement("p");
						newPar.innerHTML = title;
						
						//add to allbooks
						newDiv.appendChild(newImg);
						newDiv.appendChild(newPar);
						allbooks.appendChild(newDiv);
				}
				
				// hide singlebook,show allbooks
				document.getElementById('singlebook').style.visibility="hidden";
				document.getElementById('allbooks').style.visibility="visible";
    }
				
}
//
// loadBook() - function to display single book's cover,info,description,
//              and reviews
 
function loadBook(dir){
        // Empty allbooks
        var bookCount = document.getElementById("allbooks").childNodes.length;	
		    var allbooks = document.getElementById("allbooks");				
				for(i = 0;i<bookCount;i++){
				    var book = allbooks.firstChild;
						allbooks.removeChild(book);
				}
				// Show singlebook, hide allbooks				
				document.getElementById('singlebook').style.visibility="visible";
				document.getElementById('allbooks').style.visibility="hidden";
				
				// Set image
				cover = document.getElementById("cover");
				cover.setAttribute("src","books/"+dir+"/cover.jpg");
				// Get info,desc,& reviews
				getInfo(dir);
				getDesc(dir);
				getRev(dir);		

}
//
// getInfo() - function to request content data from php
//
function getInfo(dir){
    xhr = new XMLHttpRequest();
		xhr.onreadystatechange = getContent;
		xhr.open("GET","bestreads.php?mode=info&title="+dir, false);
		xhr.send(null);
}
//
// getContent() - function to parse JSON array to obtain
//                book's title, author, and star rating.
//
function getContent(){
    if(xhr.readyState == 4 && xhr.status == 200) {
        jpar = JSON.parse(xhr.responseText);
				document.getElementById("title").innerHTML = jpar.title;
				document.getElementById("author").innerHTML = jpar.author;
        document.getElementById("stars").innerHTML = jpar.stars;
		}		
}
//
// getDesc() - function to request description data from php
//
//
function getDesc(dir){
    x1 = new XMLHttpRequest();
		x1.onreadystatechange = getBDesc;
		x1.open("GET","bestreads.php?mode=description&title="+dir, true);
		x1.send(null);
}
//
// getBDesc() - function to display description 
//
//
function getBDesc(){
    if(x1.readyState == 4 && x1.status == 200) {
		    document.getElementById("description").innerHTML = x1.responseText;
		}
}
//
// getRev() - function to request reviews data from php
//
//				
function getRev(dir){
    x2 = new XMLHttpRequest();
		x2.onreadystatechange = getBRev;
		x2.open("GET","bestreads.php?mode=reviews&title="+dir, false);
		x2.send(null);
}
//
// gaeBRev() - function to display all reviews
//
//
function getBRev(){
    if(x2.readyState == 4 && x2.status == 200) {
		    document.getElementById("reviews").innerHTML = x2.responseText;
		}
}		