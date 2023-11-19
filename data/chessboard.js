document.addEventListener("DOMContentLoaded", function() {
  setInterval(changeColor, randomInterval());
});

function changeColor() {
  const squares = document.querySelectorAll(".square");
  squares.forEach(square => {
    square.style.backgroundColor = getRandomColor();
  });
}

function getRandomColor() {
  return `rgb(${randomValue()}, ${randomValue()}, ${randomValue()})`;
}

function randomValue() {
  return Math.floor(Math.random() * 256);
}

function randomInterval() {
  return Math.floor(Math.random() * (10000 - 3000) + 3000);
}