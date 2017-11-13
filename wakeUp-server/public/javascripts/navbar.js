'use strict';

$(document).ready(function(){

    $('.navbar-nav li a').click(function(target){
        $('.active').removeClass('active');
        $(this).addClass('active');
        target.preventDefault();
    });

});