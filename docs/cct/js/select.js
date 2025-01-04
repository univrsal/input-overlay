/* Mostly from https://www.w3schools.com/howto/howto_custom_select.asp */
var dropDowns, i, j, dropDownsCount, dropDownOptions;

/* Look for any elements with the class "custom-select": */
$(() => {
  dropDowns = document.getElementsByClassName('custom-select');
  dropDownsCount = dropDowns.length;
  for (i = 0; i < dropDownsCount; i++) {
    dropDownOptions = dropDowns[i].getElementsByTagName('select')[0];
    let dropDownOptionsCount = dropDownOptions.length;

    /* For each element, create a new DIV that will act as the selected item: */
    let a = document.createElement('DIV');
    a.setAttribute('class', 'select-selected');
    a.innerHTML =
      dropDownOptions.options[dropDownOptions.selectedIndex].innerHTML;
    dropDowns[i].appendChild(a);

    /* For each element, create a new DIV that will contain the option list: */
    let optionProxyContainerDiv = document.createElement('DIV');
    optionProxyContainerDiv.setAttribute('class', 'select-items select-hide');

    for (j = 1; j < dropDownOptionsCount; j++) {
      /* For each option in the original select element,
            create a new DIV that will act as an option item: */
      let optionProxy = document.createElement('DIV');
      optionProxy.innerHTML = dropDownOptions.options[j].innerHTML;
      optionProxy.addEventListener('click', function (e) {
        /* When an item is clicked, update the original select box,
                and the selected item: */
        var y, i, k, s, h, sl, yl;
        s = this.parentNode.parentNode.getElementsByTagName('select')[0];
        sl = s.length;
        h = this.parentNode.previousSibling;
        for (i = 0; i < sl; i++) {
          if (s.options[i].innerHTML == this.innerHTML) {
            s.selectedIndex = i;
            h.innerHTML = this.innerHTML;
            y = this.parentNode.getElementsByClassName('same-as-selected');
            yl = y.length;
            for (k = 0; k < yl; k++) {
              y[k].removeAttribute('class');
            }
            this.setAttribute('class', 'same-as-selected');
            break;
          }
        }
        h.click();
      });
      optionProxyContainerDiv.appendChild(optionProxy);
    }
    dropDowns[i].appendChild(optionProxyContainerDiv);
    a.addEventListener('click', function (e) {
      /* When the select box is clicked, close any other select boxes,
            and open/close the current select box: */
      e.stopPropagation();
      closeAllSelect(this);
      this.nextSibling.classList.toggle('select-hide');
      this.classList.toggle('select-arrow-active');
    });
  }
});

function closeAllSelect(elmnt) {
  /* A function that will close all select boxes in the document,
    except the current select box: */
  var x,
    y,
    i,
    xl,
    yl,
    arrNo = [];
  x = document.getElementsByClassName('select-items');
  y = document.getElementsByClassName('select-selected');
  xl = x.length;
  yl = y.length;
  for (i = 0; i < yl; i++) {
    if (elmnt == y[i]) {
      arrNo.push(i);
    } else {
      y[i].classList.remove('select-arrow-active');
    }
  }
  for (i = 0; i < xl; i++) {
    if (arrNo.indexOf(i)) {
      x[i].classList.add('select-hide');
    }
  }
}

/* If the user clicks anywhere outside the select box,
then close all select boxes: */
document.addEventListener('click', closeAllSelect);
