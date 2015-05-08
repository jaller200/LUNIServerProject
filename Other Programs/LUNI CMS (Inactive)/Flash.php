<!DOCTYPE HTML PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head id="ctl00_Head1">


<link href="Flash.aspx_files/en-US950.css" rel="stylesheet" type="text/css"><title>
	LEGO.com  Account Registration Intro
</title><link rel="SHORTCUT ICON" href="https://account.lego.com/favicon.ico">
   
    <script type="text/javascript">
    
    function addClickFunction(id) 
    {
        var b = document.getElementById(id);
        
        if (b && typeof(b.click) == 'undefined')
        {
        
            b.click = function() 
            {
                var result = true; 
                if (b.onclick) 
                    result = b.onclick();
                if (typeof(result) == 'undefined' || result) 
                { 
                    eval(b.href); 
                }
            }
        }
        
    }
    
    function tVis(id, show)
    {
        var ctrl = document.getElementById(id);
        if(ctrl != null)
        {
            if(show)
            {
                ctrl.style.display='inline';
            }
            else
            {
                ctrl.style.display='none';
            }
        }
    }
    
    function validate(ctrl)
    {
        var checkCtrl = document.getElementById(ctrl.getAttribute('checkCtrlId'));
        var showTick = true;
        var isValid = true;
        
            for(i =0; i <ctrl.Validators.length; i++)
            {
                isValid = ctrl.Validators[i].isvalid == '1' && isValid;
                //RequiredFieldValidator
                if(ctrl.Validators[i].initialvalue != null && ctrl.Validators[i].initialvalue != 'undefined')
                {
                    showTick = showTick && RequiredFieldValidatorEvaluateIsValid(ctrl.Validators[i]);
                }
                else
                {
                    if(ctrl.Validators[i].isvalid=='0')
                    {
                        ctrl.Validators[i].style.display='inline';
                    }
                }
                showTick = showTick && isValid;
            }
        
        if(checkCtrl != null)
        {    
            if(showTick)
            {
                checkCtrl.style.display='inline';
            }
            else
            {
                checkCtrl.style.display='none';
            }
        }
        var hideCtrl = document.getElementById(ctrl.getAttribute('hideCtrlId'));
        if(hideCtrl != null)
        {
            if(!isValid)
            {
                hideCtrl.style.display = 'none';
            }
            else
            {
            
                hideCtrl.style.display = 'inline';
            }
        }
    }
    
    function hideError(ctrl, valId)
    {
        
        var hideCtrl = document.getElementById(ctrl.getAttribute('hideCtrlId'));
        if(hideCtrl != null)
        {
            hideCtrl.style.display='inline';
        }
        var validator = document.getElementById(valId);
        if(validator != null)
        {
        
            validator.style.display='none';
        }
    }
    
    
    function hideTick(ctrl)
    {
        var checkCtrl = document.getElementById(ctrl.getAttribute('checkCtrlId'));
        if(checkCtrl != null)
        {
            checkCtrl.style.display='none';
        }
    }
        
    function showHint(ctrl)
    {
        var hintCtrl = document.getElementById(ctrl.getAttribute('hintid'));
        var descCtrl = document.getElementById(ctrl.getAttribute('descid'));
        if(hintCtrl != null && hintCtrl != 'undefined')
        {
            hintCtrl.style.display = 'block';
        }
        if(descCtrl != null && descCtrl != 'undefined')
        {
            descCtrl.style.display = 'block';
        }
    }

    function hideHint(ctrl)
    {
        var hintCtrl = document.getElementById(ctrl.getAttribute('hintid'));
        var descCtrl = document.getElementById(ctrl.getAttribute('descid'));
        if(hintCtrl != null && hintCtrl != 'undefined')
        {
            hintCtrl.style.display = 'none';
        }
        if(descCtrl != null && descCtrl != 'undefined')
        {
            descCtrl.style.display = 'none';
        }
    }
    

    </script>
    <link href="Flash.aspx_files/common.css" rel="stylesheet" type="text/css"><link href="Flash.aspx_files/Flash.css" rel="stylesheet" type="text/css"><link href="Flash.aspx_files/Registration.txt" rel="stylesheet" type="text/css"><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"><link rel="SHORTCUT ICON" href="https://account.lego.com/favicon.ico"><meta name="description" content="LEGO ID Registration Intro"><!-- SiteCatalyst code version: H.15.1. Copyright 1997-2008 Omniture, Inc. More info available at http://www.omniture.com -->
<script language="JavaScript" type="text/javascript"><!--
var s_account='legoglobal';
//--></script>
<script language="JavaScript" type="text/javascript" src="Flash.aspx_files/s_code_V005.js"></script>
<script language="JavaScript" type="text/javascript"><!--
s.pageName='account:registration:flash';
s.server='Account';
s.channel='account:registration';
s.prop1='19:00-19:14';
s.prop2='2010-09-05';
s.prop3='account';
s.prop7='en-us';
s.prop8='True';
s.prop9='IT';
s.prop10='17';
s.prop11='M';
s.prop12='IT';
var s_code=s.t();
if(s_code)document.write(s_code)//--></script>
<script language="JavaScript" type="text/javascript"><!--
if(navigator.appVersion.indexOf('MSIE')>=0)document.write(unescape('%3C')+'\!-'+'-')
//--></script>
<noscript><a href="http://www.omniture.com" title="Web Analytics"><img src="http://legobrandretail.112.2o7.net/b/ss/legodev/1/H.15.1--NS/0" height="1" width="1" border="0" alt="" /></a></noscript><!--/DO NOT REMOVE/-->
</head><body>
    <form name="aspnetForm" method="post" action="Flash.aspx?ReturnUrl=http%3a%2f%2funiverse.lego.com%2fen-us%2fbetacenter%2fdefault.aspx" id="aspnetForm">
<div>
<input name="__VIEWSTATE" id="__VIEWSTATE" value="/wEPDwULLTE5NjMwNjIwMTgPZBYCZg9kFgICBA9kFgoCAQ8WAh4HVmlzaWJsZWhkAgMPFgIeA3NyYwVMaHR0cHM6Ly9hMjQ4LmUuYWthbWFpLm5ldC9jYWNoZS5sZWdvLmNvbS9pbWFnZXMvYWNjb3VudC9pbWc5NjF4MjhfQkdfVG9wLmpwZ2QCBQ9kFgJmDxYCHglpbm5lcmh0bWwFB0xFR08gSURkAgcPZBYIZg8VAShodHRwczovL2EyNDguZS5ha2FtYWkubmV0L2NhY2hlLmxlZ28uY29tZAIBDxYCHwIFCE5ldyBVc2VyZAIHDxYCHgRocmVmBVVUT0MuYXNweD9SZXR1cm5Vcmw9aHR0cCUzYSUyZiUyZnVuaXZlcnNlLmxlZ28uY29tJTJmZW4tdXMlMmZiZXRhY2VudGVyJTJmZGVmYXVsdC5hc3B4FgJmDxYCHgRUZXh0BQtORVhUICYjMTg3O2QCCQ8WAh8EBQxMRUdPIElEIEhFTFBkAgkPFgIfAGhkZKMQreKW2YXOZlQZaRXbgwE6Cq0v" type="hidden">
</div>


<script type="text/javascript">
//<![CDATA[
function showFAQ() {  window.open('/en-us/FAQ.aspx','LEGOIDFAQ','titlebar=no, toolbar=no, menubar=no, scrollbars=yes, resizable=no, location=no, directories=no, status=no,width=580, height=650');  }//]]>
</script>

        
        <div id="wrapper">
            <img src="Flash.aspx_files/img961x28_BG_Top.jpg" id="ctl00_imgBorderTop" class="topBorder"> 
            <div id="container">
                <ul id="progress">
	</ul><h1 id="ctl00_SubHeader1_h1Logo">Raffa's ID</h1> <strong>This isn't lego Registration!</strong>

                
<script type="text/javascript" src="Flash.aspx_files/swfobject.js"></script>
<div id="Fcontent">
    <h2 id="ctl00_ContentPlaceHolder1_ImgHeader">New User</h2>
    <div id="Ftxt">
        
        <div id="flashcontent" style="height: 100%;">
            <div id="SWFID" class="flashcontent"><embed type="application/x-shockwave-flash" src="Flash.aspx_files/id_terms.swf" name="SWFID" bgcolor="#000" quality="best" allowscriptaccess="never" wmode="transparent" id="SWFID" flashvars="xmlPath=https://a248.e.akamai.net/cache.lego.com/upload/contentTemplating/AccountDictionary/otherfiles/1033/uploadB9B01708-BF4B-4E29-B2A6-3FC078A8EDD8.xml" height="289px" width="680px"></div>
	<script type="text/javascript">
		var so = new SWFObject('https://a248.e.akamai.net/cache.lego.com/flash/account/id_terms.swf', 'SWFID', '680px', '289px', '9', '#000');
		so.addParam('allowScriptAccess', 'never')
		so.addParam('wmode', 'transparent')
		so.addParam('quality', 'best')
		so.addParam('id', 'SWFID')
		so.addVariable('xmlPath', 'https://a248.e.akamai.net/cache.lego.com/upload/contentTemplating/AccountDictionary/otherfiles/1033/uploadB9B01708-BF4B-4E29-B2A6-3FC078A8EDD8.xml');
		so.write('SWFID');

	</script>
        </div>
    </div>
    <a href="signup.php" id="ctl00_ContentPlaceHolder1_AnchorAccept" class="btnGreen FacceptBtn">NEXT »</a>
    <div id="footer" class="Ffooter">
        <a href="javascript:showFAQ();">LEGO ID HELP</a>
    </div>
</div>

            </div>
        </div>
        
    </form>
</body></html>