<!DOCTYPE HTML PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head id="ctl00_Head1">


<link href="signin.aspx_files/en-US950.css" rel="stylesheet" type="text/css"><title>Raffa's  Account Sign in</title><link rel="SHORTCUT ICON" href="https://account.lego.com/favicon.ico">
   
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
    
    <script type="text/javascript">
        function redirectToForgotPassword(elem)
        {
            var validator = document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername');
            ValidatorValidate(validator);
            ValidatorUpdateDisplay(validator);
            return validator.isvalid;
        }
    </script>
<link href="signin.aspx_files/common.css" rel="stylesheet" type="text/css"><link href="signin.aspx_files/signin.css" rel="stylesheet" type="text/css"><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"><link rel="SHORTCUT ICON" href="https://account.lego.com/favicon.ico"><meta name="description" content="LEGO ID Sign in"><!-- SiteCatalyst code version: H.15.1. Copyright 1997-2008 Omniture, Inc. More info available at http://www.omniture.com -->
<script language="JavaScript" type="text/javascript"><!--
var s_account='legoglobal';
//--></script>
<script language="JavaScript" type="text/javascript" src="signin.aspx_files/s_code_V005.js"></script>
<script language="JavaScript" type="text/javascript"><!--
s.pageName='account:homepage:signin';
s.server='Account';
s.channel='account:homepage';
s.prop1='18:45-18:59';
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
    <form method="post" action="" onsubmit="javascript:return WebForm_OnSubmit();">
<div>
<input name="__LASTFOCUS" id="__LASTFOCUS" value="" type="hidden">
<input name="__EVENTTARGET" id="__EVENTTARGET" value="" type="hidden">
<input name="__EVENTARGUMENT" id="__EVENTARGUMENT" value="" type="hidden">
<input name="__VIEWSTATE" id="__VIEWSTATE" value="/wEPDwULLTE3MjU2ODY4MzYPZBYCZg9kFgICBA9kFgoCAQ8WAh4HVmlzaWJsZWhkAgMPFgIeA3NyYwVMaHR0cHM6Ly9hMjQ4LmUuYWthbWFpLm5ldC9jYWNoZS5sZWdvLmNvbS9pbWFnZXMvYWNjb3VudC9pbWc5NjF4MjhfQkdfVG9wLmpwZ2QCBQ9kFgJmDxYCHglpbm5lcmh0bWwFB0xFR08gSURkAgcPZBYIZg9kFhYCAQ8WAh8CBQdTaWduIGluZAICDxYCHgRUZXh0BQlVc2VybmFtZTpkAgQPDxYCHgxFcnJvck1lc3NhZ2UFGlBsZWFzZSBmaWxsIG91dCB0aGUgZmllbGQhZGQCBQ8WAh8DZWQCBg8PFgIfBAUcVGhlIHVzZXJuYW1lIGRvZXMgbm90IGV4aXN0LmRkAgcPFgIfAwUJUGFzc3dvcmQ6ZAIJDw8WAh8EBRpQbGVhc2UgZmlsbCBvdXQgdGhlIGZpZWxkIWRkAgoPDxYCHwMFB1NJR04gSU5kZAILDxAPFgIfAwV6S2VlcCBtZSBzaWduZWQgaW4gb24gdGhpcyBjb21wdXRlciB1bmxlc3MgSSBzaWduIG91dC4NCjxhIGlkPSJBbmNob3JMZWFybk1vcmUiIGhyZWY9ImphdmFzY3JpcHQ6c2hvd0ZBUSgpOyI+TGVhcm4gbW9yZTwvYT5kZGRkAgwPDxYCHwMFNUZvcmdvdCB5b3VyIHBhc3N3b3JkPyBUeXBlIGluIHlvdXIgdXNlcm5hbWUgYWJvdmUgYW5kZGQCDQ9kFgJmDxYCHwMFCmNsaWNrIGhlcmVkAgEPFgQfAQWLAWh0dHBzOi8vYTI0OC5lLmFrYW1haS5uZXQvY2FjaGUubGVnby5jb20vdXBsb2FkL2NvbnRlbnRUZW1wbGF0aW5nL0FjY291bnREaWN0aW9uYXJ5L2ltYWdlcy8xMDMzL3BpY0NBN0YzMzhDLTAzNTgtNDE0NC04MjdELTIzREI5RkQ0MTRFQi5naWYeA2FsdAUJTmV3IHVzZXI/ZAICDxYCHgRocmVmBW8vSW5Db250ZXh0L3JlZ2lzdHJhdGlvbi9mbGFzaC5hc3B4P1JldHVyblVybD1odHRwJTNhJTJmJTJmdW5pdmVyc2UubGVnby5jb20lMmZlbi11cyUyZmJldGFjZW50ZXIlMmZkZWZhdWx0LmFzcHgWAmYPFgIfAwULU0lHTiBVUCBOT1dkAgMPFgIfAwUMTEVHTyBJRCBIRUxQZAIJDxYCHwBoZBgBBR5fX0NvbnRyb2xzUmVxdWlyZVBvc3RCYWNrS2V5X18WAQUwY3RsMDAkQ29udGVudFBsYWNlSG9sZGVyMSRDaGVja0JveEtlZXBNZVNpZ25lZElulPICiBKtW1SDeSanQQgMHaInKnE=" type="hidden">
</div>

<script type="text/javascript">
//<![CDATA[
var theForm = document.forms['aspnetForm'];
if (!theForm) {
    theForm = document.aspnetForm;
}
function __doPostBack(eventTarget, eventArgument) {
    if (!theForm.onsubmit || (theForm.onsubmit() != false)) {
        theForm.__EVENTTARGET.value = eventTarget;
        theForm.__EVENTARGUMENT.value = eventArgument;
        theForm.submit();
    }
}
//]]>
</script>


<script src="signin.aspx_files/WebResource_002.js" type="text/javascript"></script>


<script type="text/javascript">
//<![CDATA[
function showFAQ() {  window.open('/en-us/FAQ.aspx','LEGOIDFAQ','titlebar=no, toolbar=no, menubar=no, scrollbars=yes, resizable=no, location=no, directories=no, status=no,width=580, height=650');  }//]]>
</script>

<script src="signin.aspx_files/WebResource_003.js" type="text/javascript"></script>
<script src="signin.aspx_files/WebResource.js" type="text/javascript"></script>
<script type="text/javascript">
//<![CDATA[
function WebForm_OnSubmit() {
if (typeof(ValidatorOnSubmit) == "function" && ValidatorOnSubmit() == false) return false;
return true;
}
//]]>
</script>

	<div>

		<input name="__EVENTVALIDATION" id="__EVENTVALIDATION" value="/wEWCALX37IUAtGdzMoIAtz0i6kMAq2s6qgPAuX544cIAtmtnL8KAvWf2YAIAurv5p0NmedOs/lxQdc1vYzc83G/X/OlynY=" type="hidden">
	</div>
        
        <div id="wrapper">
            <img src="signin.aspx_files/img961x28_BG_Top.jpg" id="ctl00_imgBorderTop" class="topBorder">
            <div id="container">
                <ul id="progress">
	</ul><h1 id="ctl00_SubHeader1_h1Logo">LEGO ID</h1>

                
    <div id="SIcontent">
    <div id="ctl00_ContentPlaceHolder1_Panel1" onkeypress="javascript:return WebForm_FireDefaultButton(event, 'ctl00_ContentPlaceHolder1_ImageButtonSignin')">
		
        <input name="ctl00$ContentPlaceHolder1$DefaultButton" value="" onclick='javascript:WebForm_DoPostBackWithOptions(new WebForm_PostBackOptions("ctl00$ContentPlaceHolder1$DefaultButton", "", true, "", "", false, false))' id="ctl00_ContentPlaceHolder1_DefaultButton" style="visibility: hidden; position: absolute;" type="submit">
        <h2 id="ctl00_ContentPlaceHolder1_ImgHeader">Sign in</h2>
        <img class="SIminiFig" src="signin.aspx_files/img185x285_Minifig.gif" alt="">
        <fieldset>
            <div class="field"><strong>This isn't Lego.com this is Raffa's Universe!</strong><br />
                <label for="ctl00_ContentPlaceHolder1_TextBoxUsername">Username:</label>
                <input name="Username" id="ctl00_ContentPlaceHolder1_TextBoxUsername" checkctrlid="TextBoxUsernameCheck" hidectrlid="TextBoxUsernameHide" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername');hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorUsername');" onblur="ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorUsername); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorUsername);validate(this);" onchange="validate(this);" type="text">
                <p class="error"><span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername" style="color: Red; display: none;">Please fill out the field!</span>
                    <span id="ctl00_ContentPlaceHolder1_CustomValidatorUsername" style="color: Red; display: none;">The username does not exist.</span></p>
            </div>
            <div class="field">
                <label for="ctl00_ContentPlaceHolder1_TextBoxPassword">Password:</label>
                
                    <input name="Password" id="ctl00_ContentPlaceHolder1_TextBoxPassword" checkctrlid="TextBoxPasswordCheck" hidectrlid="TextBoxPasswordHide" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword');" onchange="validate(this);" type="password">
                
                <p class="error"><span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword" style="color: Red; display: none;">Please fill out the field!</span></p>
            </div>
            <a id="ctl00_ContentPlaceHolder1_ImageButtonSignin" class="btnGreen SIsigninBtn" href='javascript:WebForm_DoPostBackWithOptions(new%20WebForm_PostBackOptions("ctl00$ContentPlaceHolder1$ImageButtonSignin",%20"",%20true,%20"",%20"",%20false,%20true))'>SIGN IN</a>            
            <p class="SIcheckBox"><input id="ctl00_ContentPlaceHolder1_CheckBoxKeepMeSignedIn" name="ctl00$ContentPlaceHolder1$CheckBoxKeepMeSignedIn" type="checkbox"><label for="ctl00_ContentPlaceHolder1_CheckBoxKeepMeSignedIn">Keep me signed in on this computer unless I sign out.
<a id="AnchorLearnMore" href="javascript:showFAQ();">Learn more</a></label></p>
            <p>
                <span id="ctl00_ContentPlaceHolder1_LabelForgetPassword">Forgot your password? Type in your username above and</span><br>
                <a onclick="return redirectToForgotPassword();" id="ctl00_ContentPlaceHolder1_LinkButtonForgotPassword" href="javascript:__doPostBack('ctl00$ContentPlaceHolder1$LinkButtonForgotPassword','')">click here</a>            </p>
        </fieldset>
	</div>
    </div>  
    <div id="SIcontentSub">
        <img class="ditsImage" src="signin.aspx_files/img865x1_Dots.gif" alt="">
        <img src="signin.aspx_files/picCA7F338C-0358-4144-827D-23DB9FD414EB.gif" id="ctl00_ContentPlaceHolder1_ImageNotAMemberYet" class="SIimgTxt" alt="New user?">
        <a href="Flash.php" id="ctl00_ContentPlaceHolder1_AnchorNotAMemberYet" class="btnBlue">SIGN UP NOW</a>
        <a class="SIfaq" href="javascript:showFAQ();">LEGO ID HELP</a>
        <img class="noWhiteSpace" src="signin.aspx_files/img865x1_Dots.gif" alt="">    </div>   
            </div>
        </div>
        
    
<script type="text/javascript">
//<![CDATA[
var Page_Validators =  new Array(document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername"), document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorUsername"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword"));
	//]]>
</script>

<script type="text/javascript">
//<![CDATA[
var ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUsername";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.errormessage = "Please fill out the field!";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.initialvalue = "";
	var ctl00_ContentPlaceHolder1_CustomValidatorUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorUsername");
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUsername";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.errormessage = "The username does not exist.";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.evaluationfunction = "CustomValidatorEvaluateIsValid";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.errormessage = "Please fill out the field!";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.initialvalue = "";
	//]]>
</script>

	
<script type="text/javascript">
//<![CDATA[
addClickFunction('ctl00_ContentPlaceHolder1_ImageButtonSignin');
var Page_ValidationActive = false;
if (typeof(ValidatorOnLoad) == "function") {
    ValidatorOnLoad();
}

function ValidatorOnSubmit() {
    if (Page_ValidationActive) {
        return ValidatorCommonOnSubmit();
    }
    else {
        return true;
    }
}
        WebForm_AutoFocus('ctl00_ContentPlaceHolder1_TextBoxUsername');//]]>
</script>
</form>
</body></html>