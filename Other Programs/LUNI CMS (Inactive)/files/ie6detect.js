$(document).ready(function () {     
    if($.browser.msie && parseInt($.browser.version) == 6 && $.cookie('universe_ie6check') != 'true'){               
        $.cookie('universe_ie6check', 'true', { expires: 7, domain: 'lego.com'});        
        document.location = "/default.aspx";
    }    
});
