'use strict';
$(document).ready( function() {

  //generate clock animations
  var now       = new Date(),
  hourDeg   = now.getHours() / 12 * 360 + now.getMinutes() / 60 * 30,
  minuteDeg = now.getMinutes() / 60 * 360 + now.getSeconds() / 60 * 6,
  secondDeg = now.getSeconds() / 60 * 360,
  stylesDeg = [
      "@-webkit-keyframes rotate-hour{from{transform:rotate(" + hourDeg + "deg);}to{transform:rotate(" + (hourDeg + 360) + "deg);}}",
      "@-webkit-keyframes rotate-minute{from{transform:rotate(" + minuteDeg + "deg);}to{transform:rotate(" + (minuteDeg + 360) + "deg);}}",
      "@-webkit-keyframes rotate-second{from{transform:rotate(" + secondDeg + "deg);}to{transform:rotate(" + (secondDeg + 360) + "deg);}}",
      "@-moz-keyframes rotate-hour{from{transform:rotate(" + hourDeg + "deg);}to{transform:rotate(" + (hourDeg + 360) + "deg);}}",
      "@-moz-keyframes rotate-minute{from{transform:rotate(" + minuteDeg + "deg);}to{transform:rotate(" + (minuteDeg + 360) + "deg);}}",
      "@-moz-keyframes rotate-second{from{transform:rotate(" + secondDeg + "deg);}to{transform:rotate(" + (secondDeg + 360) + "deg);}}"
  ].join("");

  document.getElementById("clock-animations").innerHTML = stylesDeg;

  $('.timepicker1').timepicker();

  $('.time-button').click( function(a) {
    $('#timeModal').modal('show');
    var aInfo = JSON.parse($(a.target).children('input').val());

    $('.timepicker1').val(aInfo.time);
    if (aInfo.level == "easy") {
        $('#radio1').click();
    } else if (aInfo.level == "medium") {
        $('#radio2').click();
    } else {
        $('#radio3').click();
    }

    for (var i = 0; i < aInfo.days.length; i++) {
      var id = i + 1;
      if (aInfo.days[i] == "on") {
        $('#day' + id).click();
      }
    }

  });

  //clear modal on modal hide
  $('#timeModal').on('hidden.bs.modal', function () {
      var date = new Date();
      $('.timepicker1').val(moment(date).format('h:mm A'));
      for (var i = 0; i < 7; i++) {
      var id = i + 1;

      if ($('#day' + id).is(':checked')) {
        $('#day' + id).click();
      }

      $('#radio1').click();
    }
  });

});

