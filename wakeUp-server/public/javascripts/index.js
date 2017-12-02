'use strict';

var socket = io();

$(document).ready( function() {

  socket.on('connect', function(){
    socket.emit('handshake');
  });

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

    $('#saveBtn').text('Save Alarm');
    $('#existingID').val(aInfo.identification);
    $('.timepicker1').val(aInfo.time);
    if (aInfo.level == "easy") {
        $('#radio1').click();
    } else if (aInfo.level == "medium") {
        $('#radio2').click();
    } else {
        $('#radio3').click();
    }

    $("#questionBtn").val(aInfo.question);
    $("#questionBtn").text(aInfo.question);

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
      $('#existingID').val(-1);
      $('.timepicker1').val(moment(date).format('h:mm A'));


      for (var i = 0; i < 7; i++) {
        var id = i + 1;
        if ($('#day' + id).is(':checked')) {
          $('#day' + id).click();
        }
        $('#radio1').click();
      }

      $("#questionBtn").val("Random");
      $("#questionBtn").text("Random");

      $('#saveBtn').text('Set Alarm');
  });

  $('#createBtn').click( function(e) {
    var now = new Date();
    var todayID = now.getDay() + 1;
    $('#day' + todayID).click();
  })


  //change status
  $(".alarm-status").on('change', function(e) {
    $.ajax({
        type: "POST",
        url: "/alarm/status",
        data: { 
            alarmId: $(this).val(),
            status: $(this).prop('checked'),
        }
    });
  });

  $(".question-type-dropdown li a").click( function(e) {
    var selected = $(this).text();

    $("#questionBtn").val(selected);
    $("#questionBtn").text(selected);
  });

  socket.on('triggerAlarm', function(isMCQ, question, answers) {

      $('#triggerModal').modal('show');

      var time = getCurrentTime();
      var heading = "Wake UP! It's <b>" + time + "</b>."
      $("#myTriggerModalLabel").html(heading);

      if (isMCQ == "true") {
        $("#question").html("<b>Q.</b> " + question);

        $("#op1").html("<b>A)</b> " + answers[0]);
        $("#op2").html("<b>B)</b> " + answers[1]);
        if (answers[2]) {
          $("#op3").html("<b>C)</b> " + answers[2]);
          $("#op4").html("<b>D)</b> " + answers[3]);
        }
      } else {
        $("#question").html("<b>Q.</b> " + question + " ?");
      }
  });

  socket.on('stopAlarm', function() {
      $('#triggerModal').modal('hide');
  });

});

function getCurrentTime() {
    var date = new Date();
    var hours = date.getHours() > 12 ? date.getHours() - 12 : date.getHours();
    var am_pm = date.getHours() >= 12 ? "PM" : "AM";
    hours = hours < 10 ? "0" + hours : hours;
    var minutes = date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes();
    var time = hours + ":" + minutes + " " + am_pm;
    return time;
};

