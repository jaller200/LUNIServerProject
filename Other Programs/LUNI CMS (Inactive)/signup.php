<!DOCTYPE HTML PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head id="ctl00_Head1">


<link href="signup.aspx_files/en-US950.css" rel="stylesheet" type="text/css"><title>
	LEGO.com  Account Registration Step 2
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
    
    <script type="text/javascript">
    var hideCtrls = new Array('TextBoxPasswordHide','TextBoxUserNameHide');
    function validateRequired() {
        for(i = 0; i < hideCtrls.length;i++) {
            document.getElementById(hideCtrls[i]).style.display = 'none';
        }
        var isValid = true;
        for(i =0; i <Page_Validators.length; i++) {
            if(Page_Validators[i].initialvalue != null && Page_Validators[i].initialvalue != 'undefined') {
                var validate = isParentDisplayed(document.getElementById(Page_Validators[i].controltovalidate));
                if(validate) {
                    Page_Validators[i].enabled = true;
                    isValid = isValid && RequiredFieldValidatorEvaluateIsValid(Page_Validators[i]);
                    ValidatorValidate(Page_Validators[i]);
                    ValidatorUpdateDisplay(Page_Validators[i]);
                    Page_Validators[i].isvalid = true;
                    Page_Validators[i].enabled = false;
                }
            }
        }
        return isValid;
    }
    
    function isParentDisplayed(ctrl)
    {
        var retVal = true;
        if(ctrl.style != null && ctrl.style != 'undefined')
        {
            retVal = ctrl.style.display != 'none';
        }
        if(ctrl.parentNode != null && ctrl.parentNode != 'undefined')
        {
            if(retVal)
            {
                retVal = isParentDisplayed(ctrl.parentNode);
            }
        }
        return retVal;
    }
    
    function confirmDateOfBirth()
    {
        var year = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear').options[document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear').selectedIndex].value;
        var month = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth').options[document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth').selectedIndex].value;
        var day = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay').options[document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay').selectedIndex].value;
        document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value = year + '-' + month + '-' + day;
        lockDateOfBirth();
        toggleEmailContainer();
        togglePIIContainer();
        if(validateDateOfBirth())
        {
            document.getElementById('ctl00_ContentPlaceHolder1_TextBoxEMail').focus();
        }
    }
        
    function bindDateOfBirth(dateOfBirthStr)
    {
        var dateElems = dateOfBirthStr.split('-');
        selectItem(document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear'), parseInt(dateElems[0]));
        selectItem(document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth'), parseInt(dateElems[1]));
        selectItem(document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay'), parseInt(dateElems[2]));
        toggleEmailContainer();
        togglePIIContainer();
        
    }
        
    function selectItem(dropdownlist, value)
    {
        for(i=0;i < dropdownlist.options.length;i++)
        {                
            dropdownlist.options[i].selected = (dropdownlist.options[i].value == value);
        }
    }      
    
    function validateDateOfBirth()
    {
    
        if(document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value.length == 0)
        {
            document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value = '';
        }
        
        var reqVal = document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth');
        ValidatorValidate(reqVal);
        ValidatorUpdateDisplay(reqVal);
        var rangeVal = document.getElementById('ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth');
        ValidatorValidate(rangeVal);
        ValidatorUpdateDisplay(rangeVal);
        validate(document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth'));
        return rangeVal.isvalid && reqVal.isvalid;
    }
        
    function isChild()
    {
        var dateElems = document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value.split('-');
        var dateOfBirth = new Date();
        dateOfBirth.setFullYear( (parseInt(dateElems[0])+13), (parseInt(dateElems[1])-1), parseInt(dateElems[2]) );
        return dateOfBirth > new Date();
    }
    
    function togglePIIContainer()
    {
        if(validateDateOfBirth())
       {   
            if(!isChild())
            {
                document.getElementById('PIIContainer').style.display = 'block';
            }
        }     
    }
    
    function toggleEmailContainer()
    {
       if(validateDateOfBirth())
       {   
            if(isChild())
            {
               document.getElementById('ctl00_ContentPlaceHolder1_LabelEmail_NotChild').style.display = 'none';
               document.getElementById('ctl00_ContentPlaceHolder1_LabelEmail_Child').style.display = '';
            }
            else
            {
                document.getElementById('ctl00_ContentPlaceHolder1_LabelEmail_NotChild').style.display = '';
                document.getElementById('ctl00_ContentPlaceHolder1_LabelEmail_Child').style.display = 'none';
            }
            document.getElementById('ctl00_ContentPlaceHolder1_EmailContainer').style.display = 'block';
        }
        else
        {
            document.getElementById('ctl00_ContentPlaceHolder1_EmailContainer').style.display = 'none';
        }
    }
    
    function validateUsername()
    {
        var val = document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername');
        val.enabled = true;
        ValidatorUpdateDisplay(val);
        ValidatorValidate(val);
        var isvalid = val.isvalid;
        val.enabled = false;
        val.isvalid = true;
        if(isvalid)
        {
            val = document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername');
            ValidatorUpdateDisplay(val);
            ValidatorValidate(val);
            isvalid = val.isvalid;
        }
                
        if(isvalid)
        {
            tVis('TextBoxUserNameCheck',true);
        }
        else
        {
            tVis('TextBoxUserNameHide',false);
        }
        
        return isvalid;
    }
    
    function customUsernameValidator(source, arguments)
    {
        var digits = 0;
        for(index = 0; index < arguments.Value.length; index++)
        {
            if(!isNaN(arguments.Value.charAt(index)))
            {
                digits++;
            }
        }
        arguments.IsValid = digits < 5;
    }
    
    function customPasswordValidator(source, arguments)
    {
        arguments.IsValid = arguments.Value.toString().toUpperCase() != document.getElementById('ctl00_ContentPlaceHolder1_TextBoxUserName').value.toUpperCase();
    }
    
    function checkForCookie()
    {
        var dateOfBirthStr = readCookieDateOfBirth();
        if(dateOfBirthStr != null && dateOfBirthStr != 'undefined')
        {
            var username = dateOfBirthStr.split('|')[1];
            dateOfBirthStr = dateOfBirthStr.split('|')[0];
            if(username == document.getElementById('ctl00_ContentPlaceHolder1_TextBoxUserName').value)
            {
                document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value = dateOfBirthStr;
                lockDateOfBirth();
                bindDateOfBirth(dateOfBirthStr);
            }
            else
            { 
                document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear').disabled = false;
                document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth').disabled = false;
                document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay').disabled = false;
            }
         }                         
    }
    
    function readCookieDateOfBirth()
    {
       	var nameEQ = "dateOfBirth=";
       	if(document.cookie != null)
       	{
            var ca = document.cookie.split(';');
            for(var i=0;i < ca.length;i++) {
	            var c = ca[i];
	            while (c.charAt(0)==' ') c = c.substring(1,c.length);
	            if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
            }
        }    
        return null;
    }
    
    function lockDateOfBirth()
    {
        if(validateDateOfBirth())
        {        
            document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear').disabled = true;
            document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth').disabled = true;
            document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay').disabled = true;
            var date = new Date();
	        date.setTime(date.getTime()+(1*60*60*1000));
		    var expires = "; expires=" +date.toGMTString();
            document.cookie = 'dateOfBirth='+ document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth').value + '|' + document.getElementById('ctl00_ContentPlaceHolder1_TextBoxUserName').value + expires + '; path=/';
            toggleEmailContainer();
            document.getElementById('AnchorConfirmDateOfBirth').style.display = 'none';
        }
    }
    
    function showConfirmDateOfBirth()
    {
        var yElem = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListYear');
        var mElem = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListMonth');
        var dElem = document.getElementById('ctl00_ContentPlaceHolder1_DropDownListDay');
        
        if(yElem.options[yElem.selectedIndex].value != '-1' && mElem.options[mElem.selectedIndex].value != '-1' && dElem.options[dElem.selectedIndex].value != '-1')
        {
            var hideCtrl = document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirthHide');
            if(hideCtrl != null)
            {
                hideCtrl.style.display='inline';
            }
            
            hideCtrl = document.getElementById('DateOfBirthHint');
            if(hideCtrl != null)
            {
                hideCtrl.style.display='none';
            }
        }
    }
    

    function ddFocus()
    {
        var ctrl = document.getElementById('ctl00_ContentPlaceHolder1_TextBoxDateOfBirth');
        for(i =0; i <ctrl.Validators.length; i++)
        {
            isValid = ctrl.Validators[i].style.display = 'none';
        }
    }
    
    function textboxUsernameFocus()
    {
        tVis('TextboxErrors',false);
        document.getElementById('ctl00_ContentPlaceHolder1_TextBoxUserNameCheck').style.display = 'none';
        document.getElementById('UsernameHint').style.display = 'block';
        document.getElementById('UsernameDesc').style.display = 'block';
        document.getElementById('UsernameMsg').style.display = 'none';
    }
    
    function textboxUsernameBlur()
    {
        tVis('UsernameHint',false);
        tVis('UsernameDesc',false);
        tVis('TextboxErrors',true);
    }
    
    function verifyUsername()
    {
        __doPostBack('ctl00_ContentPlaceHolder1_UpdatePanel1','');
    }

    function customPasswordInvalidValidator(source, arguments) 
    {
        arguments.IsValid = (invalidPwd.indexOf(',' + arguments.Value.toString().toUpperCase() + ',') == -1)
    }

    function validateCountrySelection(source, arguments) {
        var countryList = document.getElementById("ctl00_ContentPlaceHolder1_DropDownListCountry");
        if(true)
        {
            arguments.IsValid = (countryList[countryList.selectedIndex].value != 'KR');
        }
        else
        {
            arguments.IsValid = true;
        }
    }
 </script>
<link href="signup.aspx_files/common.css" rel="stylesheet" type="text/css"><link href="signup.aspx_files/signup.css" rel="stylesheet" type="text/css"><link href="signup.aspx_files/Registration.txt" rel="stylesheet" type="text/css"><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"><link rel="SHORTCUT ICON" href="https://account.lego.com/favicon.ico"><meta name="description" content="LEGO ID Registration"><!-- SiteCatalyst code version: H.15.1. Copyright 1997-2008 Omniture, Inc. More info available at http://www.omniture.com -->
<script language="JavaScript" type="text/javascript"><!--
var s_account='legoglobal';
//--></script>
<script language="JavaScript" type="text/javascript" src="signup.aspx_files/s_code_V005.js"></script>
<script language="JavaScript" type="text/javascript"><!--
s.pageName='account:registration:signup';
s.server='Account';
s.channel='account:registration';
s.prop1='19:30-19:44';
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
    <form method="post" action="doreg.php">
<div>
<input name="__LASTFOCUS" id="__LASTFOCUS" value="" type="hidden">
<input name="__EVENTTARGET" id="__EVENTTARGET" value="" type="hidden">
<input name="__EVENTARGUMENT" id="__EVENTARGUMENT" value="" type="hidden">
<input name="__VIEWSTATE" id="__VIEWSTATE" value="/wEPDwUKMjA2NTIzMDQzMw9kFgJmD2QWAgIED2QWCgIBDxYCHgdWaXNpYmxlaGQCAw8WAh4Dc3JjBUxodHRwczovL2EyNDguZS5ha2FtYWkubmV0L2NhY2hlLmxlZ28uY29tL2ltYWdlcy9hY2NvdW50L2ltZzk2MXgyOF9CR19Ub3AuanBnZAIFD2QWAmYPFgIeCWlubmVyaHRtbAUHTEVHTyBJRGQCBw9kFgICAw9kFkACAQ8WAh8CBQdTaWduIHVwZAICD2QWAmYPZBYSZg8PFgIeBFRleHQFCVVzZXJuYW1lOmRkAgIPFgwfAQVJaHR0cHM6Ly9hMjQ4LmUuYWthbWFpLm5ldC9jYWNoZS5sZWdvLmNvbS9pbWFnZXMvYWNjb3VudC9pbWcxNHgxNF9UaWNrLmdpZh4Fc3R5bGUFDWRpc3BsYXk6bm9uZTseBmhpbnRJZAUMVXNlcm5hbWVIaW50HgZkZXNjSWQFDFVzZXJuYW1lRGVzYx4KaGlkZUN0cmxJZAUTVGV4dEJveFVzZXJOYW1lSGlkZR4LY2hlY2tDdHJsSWQFLmN0bDAwX0NvbnRlbnRQbGFjZUhvbGRlcjFfVGV4dEJveFVzZXJOYW1lQ2hlY2tkAgMPDxYCHwMFalRoaXMgYWxsb3dzIHlvdSB0byBzaWduIGluIHRvIGFsbCBMRUdPLmNvbSBhcHBsaWNhdGlvbnMuIERvbid0IHVzZSB5b3VyIHJlYWwgbmFtZSBvciBwZXJzb25hbCBpbmZvcm1hdGlvbi5kZAIEDw8WAh8DBW9Vc2UgMiB0byAzMCBjaGFyYWN0ZXJzLCB3aXRoIG5vIHNwYWNlcy4gWW91IG1heSB1c2UgbGV0dGVycyAoYS16LEEtWiksICJfIiwgIi0iIGFuZCBtYXhpbXVtIG9mIDQgbnVtYmVycyAoMC05KS5kZAIGDw8WAh4MRXJyb3JNZXNzYWdlBR1UaGlzIGluZm9ybWF0aW9uIGlzIHJlcXVpcmVkLmRkAgcPDxYEHhRWYWxpZGF0aW9uRXhwcmVzc2lvbgV9XihbQS1aYS16Xy1dezF9W0EtWmEtejAtOV8tXXsxLDMwfSl8KFtBLVphLXowLTlfLV17MSwzMH1bQS1aYS16Xy1dezF9KXwoW0EtWmEtejAtOV8tXXsxLDMwfVtBLVphLXpfLV17MX1bQS1aYS16MC05Xy1dezEsMzB9KSQfCQUpVGhlIHVzZXJuYW1lIGNvbnRhaW5zIGludmFsaWQgY2hhcmFjdGVycy5kZAIIDw8WAh8JBSpUaGUgdXNlcm5hbWUgY29udGFpbnMgbW9yZSB0aGFuIDQgbnVtYmVycy5kZAIJDw8WBB8DBTBUaGUgdXNlcm5hbWUgaXMgbm90IGF2YWlsYWJsZS4gVHJ5IHRoaXMgaW5zdGVhZDofAGhkZAIKDw8WAh8AaGRkAgMPDxYCHwMFCVBhc3N3b3JkOmRkAgQPD2QWAh4Hb25rZXl1cAUdY2hlY2tQd2RTdHJlbmd0aCh0aGlzLnZhbHVlKTtkAgUPZBYCAgEPFgIfAwURUGFzc3dvcmQgc3RyZW5ndGhkAgYPDxYCHwMFigFGb3IgYSBzdHJvbmcgcGFzc3dvcmQsIHVzZSBsZXR0ZXJzIGFuZCBudW1iZXJzLCBhbmQgbWl4IGxvd2VyY2FzZSBhbmQgdXBwZXJjYXNlIGxldHRlcnMuIERvbid0IHVzZSB5b3VyIG5hbWUgb3IgdXNlcm5hbWUgYXMgeW91ciBwYXNzd29yZC5kZAIHDw8WAh8DBWBVc2UgNiB0byA0MCBjaGFyYWN0ZXJzLCBubyBzcGFjZXMuIElmIHlvdSB1c2UgY2FwaXRhbCBsZXR0ZXJzLCByZW1lbWJlciB0byB0eXBlIHRoZW0gZXZlcnkgdGltZS5kZAIIDw8WAh8JBR1UaGlzIGluZm9ybWF0aW9uIGlzIHJlcXVpcmVkLmRkAgkPDxYCHwkFMFRoZSBwYXNzd29yZCBjYW5ub3QgYmUgdGhlIHNhbWUgYXMgdGhlIHVzZXJuYW1lLmRkAgoPDxYCHwkFNVlvdXIgcGFzc3dvcmQgaXMgbm90IHNhZmUuIFBsZWFzZSBjaG9vc2UgYW5vdGhlciBvbmUuZGQCCw8PFgQfCgUHLns2LDQwfR8JBTVZb3VyIHBhc3N3b3JkIGlzIG5vdCBzYWZlLiBQbGVhc2UgY2hvb3NlIGFub3RoZXIgb25lLmRkAgwPDxYCHwMFEFJldHlwZSBQYXNzd29yZDpkZAIODw8WAh8JBR1UaGlzIGluZm9ybWF0aW9uIGlzIHJlcXVpcmVkLmRkAg8PDxYCHwkFIlBsZWFzZSB2ZXJpZnkgeW91ciBw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R08gdGFrZXMgb25saW5lIGNoaWxkIHNhZmV0eSB2ZXJ5IHNlcmlvdXNseS5kZAIWDw8WBh8JBRFOb3QgYSB2YWxpZCBkYXRlLh4MTWluaW11bVZhbHVlBQowMDAxLzAxLzAxHgxNYXhpbXVtVmFsdWUFCjIwMTAvMDkvMDVkZAIXDw8WAh8JBSJQbGVhc2UgY29uZmlybSB5b3VyIGRhdGUgb2YgYmlydGghZGQCGA9kFghmDw8WAh8DBQdFLW1haWw6ZGQCAQ8PFgIfAwUVWW91ciBQYXJlbnQncyBFLW1haWw6ZGQCAw8PFgIfCQUdVGhpcyBpbmZvcm1hdGlvbiBpcyByZXF1aXJlZC5kZAIEDw8WBB8JBSNUaGlzIGlzIG5vdCBhIHZhbGlkIGUtbWFpbCBhZGRyZXNzLh8KBZ0BW0EtWmEtejAtOSEjJCUmJyorLz0/Xl9ge3x9fi1dKyg/OlwuW0EtWmEtejAtOSEjJCUmJyorLz0/Xl9ge3x9fi1dKykqQCg/OltBLVphLXowLTldKD86W0EtWmEtejAtOS1dKltBLVphLXowLTldKT9cLikrW0EtWmEtejAtOV0oPzpbQS1aYS16MC05LV0qW0EtWmEtejAtOV0pP2RkAhkPDxYCHwMFCENvdW50cnk6ZGQCGg8QDxYGHwxnHg5EYXRhVmFsdWVGaWVsZAUEQ29kZR4NRGF0YVRleHRGaWVsZAUETmFtZWQQFd8BEFBsZWFzZSBjaG9vc2UuLi4LQWZnaGFuaXN0YW4HQWxiYW5pYQdBbGdlcmlhB0FuZG9ycmEGQW5nb2xhCEFuZ3VpbGxhE0FudGlndWEgYW5kIEJhcmJ1ZGEJQXJnZW50aW5hB0FybWVuaWEFQXJ1YmEJQXVzdHJhbGlhB0F1c3RyaWEKQXplcmJhaWphbgdCYWhhbWFzB0JhaHJhaW4KQmFuZ2xhZGVzaAhCYXJiYWRvcwdCZWxhcnVzB0JlbGdpdW0GQmVsaXplBUJlbmluB0Jlcm11ZGEGQmh1dGFuB0JvbGl2aWETQm9zbmlhLUhlcnR6YWdvdmluYQhCb3Rzd2FuYQZCcmF6aWwWQnJpdGlzaCBWaXJnaW4gSXNsYW5kcxBCcnVuZWkgRGFydXNzYWxtCEJ1bGdhcmlhDEJ1cmtpbmEgRmFzbw9CdXJtYSAoTXlhbm1hcikHQnVydW5kaQhDYW1ib2RpYQhDYW1lcm9vbgZDYW5hZGEKQ2FwZSBWZXJkZQ5DYXltYW4gSXNsYW5kcxhDZW50cmFsIEFmcmljYW4gUmVwdWJsaWMEQ2hhZAVDaGlsZQVDaGluYRBDaHJpc3RtYXMgSXNsYW5kDUNvY29zIElzbGFuZHMIQ29sb21iaWERQ29tb3JvcyAmIE1heW90dGUFQ29uZ28MQ29vayBJc2xhbmRzCkNvc3RhIFJpY2EHQ3JvYXRpYQRDdWJhBkN5cHJ1cw5DemVjaCBSZXB1YmxpYwdEZW5tYXJrCERqaWJvdXRpCERvbWluaWNhEkRvbWluaWNhbiBSZXB1YmxpYwpFYXN0IFRpbW9yB0VjdWFkb3IFRWd5cHQLRWwgU2FsdmFkb3IRRXF1YXRvcmlhbCBHdWluZWEHRXN0b25pYQhFdGhpb3BpYRBGYWxrbGFuZCBJc2xhbmRzDUZhcm9lIElzbGFuZHMMRmlqaSBJc2xhbmRzB0ZpbmxhbmQGRnJhbmNlDUZyZW5jaCBHdWlhbmEQRnJlbmNoIFBvbHluZXNpYRFHYWJvbmVzZSBSZXB1YmxpYwZHYW1iaWEHR2VvcmdpYQdHZXJtYW55BUdoYW5hCUdpYnJhbHRhcgZHcmVlY2UJR3JlZW5sYW5kEEdyZW5hZGEvQ2Fycmljb3UKR3VhZGVsb3VwZQRHdWFtCUd1YXRlbWFsYQZHdWluZWENR3VpbmVhLUJpc3NhdQZHdXlhbmEFSGFpdGkISG9uZHVyYXMJSG9uZyBLb25nB0h1bmdhcnkHSWNlbGFuZAVJbmRpYQlJbmRvbmVzaWEESXJhbgRJcmFxB0lyZWxhbmQGSXNyYWVsBUl0YWx5C0l2b3J5IENvYXN0B0phbWFpY2EFSmFwYW4GSm9yZGFuCUthemFrc3RhbgVLZW55YRlLaXJpYmF0aSwgR2lsYmVydCBJc2xhbmRzBkt1d2FpdA9LeXJneXogUmVwdWJsaWMETGFvcwZMYXR2aWEHTGViYW5vbgdMZXNvdGhvB0xpYmVyaWEFTGlieWENTGllY2h0ZW5zdGVpbglMaXRodWFuaWEKTHV4ZW1ib3VyZwVNYWNhbwlNYWNlZG9uaWEKTWFkYWdhc2NhcgZNYWxhd2kITWFsYXlzaWEITWFsZGl2ZXMETWFsaQVNYWx0YQpNYXJ0aW5pcXVlCk1hdXJpdGFuaWEJTWF1cml0aXVzBk1leGljbwpNaWNyb25lc2lhB01vbGRvdmEGTW9uYWNvCE1vbmdvbGlhCk1vbnRzZXJyYXQHTW9yb2NjbwpNb3phbWJpcXVlEk4uIE1hcmlhbmEgSXNsYW5kcwdOYW1pYmlhBU5hdXJ1BU5lcGFsC05ldGhlcmxhbmRzFE5ldGhlcmxhbmRzIEFudGlsbGVzDU5ldyBDYWxlZG9uaWELTmV3IFplYWxhbmQJTmljYXJhZ3VhBU5pZ2VyB05pZ2VyaWELTml1ZSBJc2xhbmQLTm9ydGggS29yZWEQTm9ydGhlcm4gSXJlbGFuZAZOb3J3YXkET21hbghQYWtpc3RhbgVQYWxhdQZQYW5hbWEQUGFwdWEgTmV3IEd1aW5lYQhQYXJhZ3VheQRQZXJ1C1BoaWxpcHBpbmVzEFBpdGNhaXJuIElzbGFuZHMGUG9sYW5kCFBvcnR1Z2FsC1B1ZXJ0byBSaWNvBVFhdGFyEFJldW5pb24gKEZyYW5jZSkHUm9tYW5pYQZSdXNzaWEGUndhbmRhClNhbiBNYXJpbm8TU2FvIFRvbWUgJiBQcmluY2lwZQxTYXVkaSBBcmFiaWEHU2VuZWdhbBVTZXJiaWEgYW5kIE1vbnRlbmVncm8KU2V5Y2hlbGxlcwxTaWVycmEgTGVvbmUJU2luZ2Fwb3JlCFNsb3Zha2lhCFNsb3ZlbmlhD1NvbG9tb24gSXNsYW5kcwdTb21hbGlhDFNvdXRoIEFmcmljYQtTb3V0aCBLb3JlYQVTcGFpbglTcmkgTGFua2EJU3QgSGVsZW5hElN0IEtpdHRzIGFuZCBOZXZpcwhTdCBMdWNpYRRTdCBQaWVycmUgJiBNaXF1ZWxvbhVTdCBWaW5jZW50L0dyZW5hZGluZXMFU3VkYW4IU3VyaW5hbWUJU3dhemlsYW5kBlN3ZWRlbgtTd2l0emVybGFuZAVTeXJpYQZUYWl3YW4KVGFqaWtpc3RhbghUYW56YW5pYQhUaGFpbGFuZBFUb2dvbGVzZSBSZXB1YmxpYwVUb25nYRNUcmluaWRhZCBhbmQgVG9iYWdvB1R1bmlzaWEGVHVya2V5DFR1cmttZW5pc3RhbhBUdXJrcyBhbmQgQ2FpY29zFlR1dmFsdSwgRWxsaWNlIElzbGFuZHMGVWdhbmRhB1VrcmFpbmUUVW5pdGVkIEFyYWIgRW1pcmF0ZXMOVW5pdGVkIEtpbmdkb20NVW5pdGVkIFN0YXRlcwdVcnVndWF5ClV6YmVraXN0YW4HVmFudWF0dQxWYXRpY2FuIENpdHkJVmVuZXp1ZWxhB1ZpZXRuYW0NV2VzdGVybiBTYW1vYQVZZW1lbgVaYWlyZQZaYW1iaWEIWmltYmFid2UV3wECLTECQUYCQUwCRFoCQUQCQU8CQUkCQUcCQVICQU0CQVcCQVUCQVQCQVoCQlMCQkgCQkQCQkICQlkCQkUCQloCQkoCQk0CQlQCQk8CQkECQlcCQlICVkcCQk4CQkcCQkYCTU0CQkkCS0gCQ00CQ0ECQ1YCS1kCQ0YCVEQCQ0wCQ04CQ1gCQ0MCQ08CS00CQ0cCQ0sCQ1ICSFICQ1UCQ1kCQ1oCREsCREoCRE0CRE8CVFACRUMCRUcCU1YCR1ECRUUCRVQCRksCRk8CRkoCRkkCRlICR0YCUEYCR0ECR00CR0UCREUCR0gCR0kCR1ICR0wCR0QCR1ACR1UCR1QCR04CR1cCR1kCSFQCSE4CSEsCSFUCSVMCSU4CSUQCSVICSVECSUUCSUwCSVQCQ0kCSk0CSlACSk8CS1oCS0UCS0kCS1cCS0cCTEECTFYCTEICTFMCTFICTFkCTEkCTFQCTFUCTU8CTUsCTUcCTVcCTVkCTVYCTUwCTVQCTVECTVICTVUCTVgCRk0CTUQCTUMCTU4CTVMCTUECTVoCTVACTkECTlICTlACTkwCQU4CTkMCTloCTkkCTkUCTkcCTlUCS1ACTkQCTk8CT00CUEsCUFcCUEECUEcCUFkCUEUCUEgCUE4CUEwCUFQCUFICUUECUkUCUk8CUlUCUlcCU00CU1QCU0ECU04CWVUCU0MCU0wCU0cCU0sCU0kCU0ICU08CWkECS1ICRVMCTEsCU0gCS04CTEMCUE0CVkMCU0QCU1ICU1oCU0UCQ0gCU1kCVFcCVEoCVFoCVEgCVEcCVE8CVFQCVE4CVFICVE0CVEMCVFYCVUcCVUECQUUCVUsCVVMCVVkCVVoCVlUCVkECVkUCVk4CV1MCWUUCWlICWk0CWlcUKwPfAWdnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dnZ2dkZAIbDw8WAh8JBboBVG8gc2lnbiB1cCBmb3IgYSBMRUdPIElEIGluIFNvdXRoIEtvcmVhLCA8YSBocmVmPSdodHRwczovL2FjY291bnQubGVnby5jb20va28ta3IvcmVnaXN0cmF0aW9uL2ZsYXNoLmFzcHg/UmV0dXJuVXJsPWh0dHA6Ly91bml2ZXJzZS5sZWdvLmNvbS9lbi11cy9iZXRhY2VudGVyL2RlZmF1bHQuYXNweCc+Y2xpY2sgaGVyZS48L2E+ZGQCHA8PFgIfCQUdVGhpcyBpbmZvcm1hdGlvbiBpcyByZXF1aXJlZC5kZAIdDw8WAh8DBQdHZW5kZXI6ZGQCHg8QDxYCHwxnZA8WAmYCARYCEAUETWFsZQUBTWcQBQZGZW1hbGUFAUZnZGQCHw8PFgIfCQUdVGhpcyBpbmZvcm1hdGlvbiBpcyByZXF1aXJlZC5kZAIgDw8WAh8DBQZTVUJNSVRkZAIhDxYCHwMFDExFR08gSUQgSEVMUGQCCQ8WAh8AaGRk7r7UI4e/uDdO+QVaiVLkrX4ld4k=" type="hidden">
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


<script src="signup.aspx_files/WebResource_002.js" type="text/javascript"></script>


<script type="text/javascript">
//<![CDATA[
function showFAQ() {  window.open('/en-us/FAQ.aspx','LEGOIDFAQ','titlebar=no, toolbar=no, menubar=no, scrollbars=yes, resizable=no, location=no, directories=no, status=no,width=580, height=650');  }//]]>
</script>

<script src="signup.aspx_files/PwdStrength.js" type="text/javascript"></script>
<script src="signup.aspx_files/ScriptResource.js" type="text/javascript"></script>
<script src="signup.aspx_files/ScriptResource_003.js" type="text/javascript"></script>
<script src="signup.aspx_files/ScriptResource_002.js" type="text/javascript"></script>
<script src="signup.aspx_files/WebResource.js" type="text/javascript"></script>
<script type="text/javascript">
//<![CDATA[
function WebForm_OnSubmit() {
if (typeof(ValidatorOnSubmit) == "function" && ValidatorOnSubmit() == false) return false;
return true;
}
//]]>
</script>

	<div>

		<input name="__EVENTVALIDATION" id="__EVENTVALIDATION" value="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" type="hidden">
	</div>
        
        <div id="wrapper">
            <img src="signup.aspx_files/img961x28_BG_Top.jpg" id="ctl00_imgBorderTop" class="topBorder">
            <div id="container">
              <h1 id="ctl00_SubHeader1_h1Logo">EGO ID</h1>

                
<script type="text/javascript">
//<![CDATA[
Sys.WebForms.PageRequestManager._initialize('ctl00$ContentPlaceHolder1$ScriptManager1', document.getElementById('aspnetForm'));
Sys.WebForms.PageRequestManager.getInstance()._updateControls(['tctl00$ContentPlaceHolder1$UpdatePanel1'], [], [], 90);
	//]]>
</script>

<div id="ctl00_ContentPlaceHolder1_Panel1" onkeypress="javascript:return WebForm_FireDefaultButton(event, 'ctl00_ContentPlaceHolder1_ImageButtonRegister')">
		    
    <input name="ctl00$ContentPlaceHolder1$DefaultButton" value="" onclick='return validateRequired();WebForm_DoPostBackWithOptions(new WebForm_PostBackOptions("ctl00$ContentPlaceHolder1$DefaultButton", "", true, "", "", false, false))' id="ctl00_ContentPlaceHolder1_DefaultButton" style="visibility: hidden; position: absolute;" type="submit">
    <div id="RESUcontent">
        <h2 id="ctl00_ContentPlaceHolder1_ImgHeader">Sign up</h2>
        <fieldset>
            <div class="field">
                <div id="ctl00_ContentPlaceHolder1_UpdatePanel1">
<label for="ctl00_ContentPlaceHolder1_TextBoxUserName"><span id="ctl00_ContentPlaceHolder1_LabelUsername">Username:</span></label>
                        <input name="ctl00$ContentPlaceHolder1$TextBoxUserName" id="ctl00_ContentPlaceHolder1_TextBoxUserName" onchange="javascript:checkForCookie();validate(this);" onblur="javascript:textboxUsernameBlur();verifyUsername();ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords);validate(this);ValidatorValidate(ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername);ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorUsername); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorUsername);" onfocus="javascript:textboxUsernameFocus();hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords');hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername');hideError(this,'ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername');hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorUsername');" checkctrlid="TextBoxUserNameCheck" hidectrlid="TextBoxUserNameHide" type="text">
                        <img src="signup.aspx_files/img14x14_Tick.gif" id="ctl00_ContentPlaceHolder1_TextBoxUserNameCheck" class="RESUinputOk" style="display: none;" hintid="UsernameHint" descid="UsernameDesc" hidectrlid="TextBoxUserNameHide" checkctrlid="ctl00_ContentPlaceHolder1_TextBoxUserNameCheck">
                        <div style="display: inline;" id="TextBoxUserNameHide">
                            <div style="display: block;" id="UsernameHint" class="hintContainer RESUhintUsername">
                                <img class="hintContainerArrow" src="signup.aspx_files/img23x17_ArrowBubble.gif" alt="">
                                <p><span id="ctl00_ContentPlaceHolder1_LabelUsernameHint">This allows you to sign in to all Raffa's Server applications. Don't use lego.com id</span></p>
                            </div>
                        </div>
                        <p style="display: block;" id="UsernameDesc" class="RESUtxt"><span id="ctl00_ContentPlaceHolder1_LabelUsernameDesc">Use 2 to 30 characters, with no spaces. You may use letters (a-z,A-Z), "_", "-" and maximum of 4 numbers (0-9).</span></p>
                       
                        <p style="display: none;" class="error" id="TextboxErrors">
                            <span id="ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords" style="color: Red; display: none;"></span>
                            <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername" style="color: Red; display: none;">This information is required.</span>
                            <span id="ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername" style="color: Red; display: none;">The username contains invalid characters.</span>
                            <span id="ctl00_ContentPlaceHolder1_CustomValidatorUsername" style="color: Red; display: none;">The username contains more than 4 numbers.</span>
                            
                        </p>
                         <p style="display: none;" id="UsernameMsg" class="error RESUerrorUsername">
                            
                            
                        </p>
                        
		</div>
            </div>
            <div class="field">
                <label for="ctl00_ContentPlaceHolder1_TextBoxPassword"><span id="ctl00_ContentPlaceHolder1_LabelPassword">Password:</span></label>
                <input name="ctl00$ContentPlaceHolder1$TextBoxPassword" id="ctl00_ContentPlaceHolder1_TextBoxPassword" hintid="PasswordHint" descid="PasswordDesc" onkeyup="checkPwdStrength(this.value);" onblur="hideHint(this);validate(this);ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername);ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorPassword); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorPassword);ValidatorValidate(ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword);" onfocus="showHint(this);hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword');hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername');hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorPassword');hideError(this,'ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword');" checkctrlid="TextBoxPasswordCheck" hidectrlid="TextBoxPasswordHide" onchange="validate(this);" type="password">
                <img id="TextBoxPasswordCheck" class="RESUinputOk" src="signup.aspx_files/img14x14_Tick.gif" alt="">
                <div id="TextBoxPasswordHide">
                    
<div id="pwdStrengthContainer">
    <p>Password strength</p>
    <div id="ctl00_ContentPlaceHolder1_PasswordStrength1_pwdStrength" class="pwStrength">
        <img id="pwdStrengthImg" src="signup.aspx_files/img6x6_StrengthGreen.gif" alt="">
    </div>
</div>
                    <div id="PasswordHint" class="hintContainerSmall RESUhintPw">
                        <img class="hintContainerArrow" src="signup.aspx_files/img23x17_ArrowBubble.gif" alt="">
                        <p><span id="ctl00_ContentPlaceHolder1_LabelPasswordHint">For
 a strong password, use letters and numbers, and mix lowercase and 
uppercase letters. Don't use your name or username as your password.</span></p>
                    </div>
                </div>
                <p id="PasswordDesc" class="RESUtxt"><span id="ctl00_ContentPlaceHolder1_LabelPasswordDesc">Use 6 to 40 characters, no spaces. If you use capital letters, remember to type them every time.</span></p>
                <p class="error">
                    <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword" style="color: Red; display: none;">This information is required.</span>
                    <span id="ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername" style="color: Red; display: none;">The password cannot be the same as the username.</span>
                    <span id="ctl00_ContentPlaceHolder1_CustomValidatorPassword" style="color: Red; display: none;">Your password is not safe. Please choose another one.</span>
                    <span id="ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword" style="color: Red; display: none;">Your password is not safe. Please choose another one.</span>
                </p>
            </div>
            <div class="field">
                <label for="ctl00_ContentPlaceHolder1_TextBoxRetypePassword"><span id="ctl00_ContentPlaceHolder1_LabelRetypePassword">Retype Password:</span></label>
                <input name="ctl00$ContentPlaceHolder1$TextBoxRetypePassword" id="ctl00_ContentPlaceHolder1_TextBoxRetypePassword" checkctrlid="TextBoxRetypePasswordCheck" hidectrlid="TextBoxRetypePasswordHide" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword');hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword');" onblur="ValidatorValidate(ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword);validate(this);" onchange="validate(this);" type="password">
                <img id="TextBoxRetypePasswordCheck" class="RESUinputOk" src="signup.aspx_files/img14x14_Tick.gif" alt="">
                <p class="error">
                    <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword" style="color: Red; display: none;">This information is required.</span>
                    <span id="ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword" style="color: Red; display: none;">Please verify your password again!</span>
                </p>
            </div>
        </fieldset>
        <div class="seperator RESUseperator"></div>
        <fieldset>
            <div class="field RESUbirthField">
                <label for="ctl00_ContentPlaceHolder1_DropDownListMonth" class="RESUbirthLabel"><span id="ctl00_ContentPlaceHolder1_LabelDateOfBirth">Date of Birth:</span></label>
                <select name="ctl00$ContentPlaceHolder1$DropDownListMonth" id="ctl00_ContentPlaceHolder1_DropDownListMonth" hintid="DateOfBirthHint" onchange="showConfirmDateOfBirth();" onfocus="ddFocus();showHint(this);" onblur="hideHint(this);">
			<option selected="selected" value="-1">-</option>
			<option value="1">January</option>
			<option value="2">February</option>
			<option value="3">March</option>
			<option value="4">April</option>
			<option value="5">May</option>
			<option value="6">June</option>
			<option value="7">July</option>
			<option value="8">August</option>
			<option value="9">September</option>
			<option value="10">October</option>
			<option value="11">November</option>
			<option value="12">December</option>

		</select>
                <select name="ctl00$ContentPlaceHolder1$DropDownListDay" id="ctl00_ContentPlaceHolder1_DropDownListDay" class="birthDayDropdown" hintid="DateOfBirthHint" onchange="showConfirmDateOfBirth();" onfocus="ddFocus();showHint(this);" onblur="hideHint(this);">
			<option selected="selected" value="-1">-</option>
			<option value="1">1</option>
			<option value="2">2</option>
			<option value="3">3</option>
			<option value="4">4</option>
			<option value="5">5</option>
			<option value="6">6</option>
			<option value="7">7</option>
			<option value="8">8</option>
			<option value="9">9</option>
			<option value="10">10</option>
			<option value="11">11</option>
			<option value="12">12</option>
			<option value="13">13</option>
			<option value="14">14</option>
			<option value="15">15</option>
			<option value="16">16</option>
			<option value="17">17</option>
			<option value="18">18</option>
			<option value="19">19</option>
			<option value="20">20</option>
			<option value="21">21</option>
			<option value="22">22</option>
			<option value="23">23</option>
			<option value="24">24</option>
			<option value="25">25</option>
			<option value="26">26</option>
			<option value="27">27</option>
			<option value="28">28</option>
			<option value="29">29</option>
			<option value="30">30</option>
			<option value="31">31</option>

		</select>
                <select name="ctl00$ContentPlaceHolder1$DropDownListYear" id="ctl00_ContentPlaceHolder1_DropDownListYear" class="birthYearDropdown" hintid="DateOfBirthHint" onchange="showConfirmDateOfBirth();" onfocus="ddFocus();showHint(this);" onblur="hideHint(this);">
			<option selected="selected" value="-1">-</option>
			<option value="2010">2010</option>
			<option value="2009">2009</option>
			<option value="2008">2008</option>
			<option value="2007">2007</option>
			<option value="2006">2006</option>
			<option value="2005">2005</option>
			<option value="2004">2004</option>
			<option value="2003">2003</option>
			<option value="2002">2002</option>
			<option value="2001">2001</option>
			<option value="2000">2000</option>
			<option value="1999">1999</option>
			<option value="1998">1998</option>
			<option value="1997">1997</option>
			<option value="1996">1996</option>
			<option value="1995">1995</option>
			<option value="1994">1994</option>
			<option value="1993">1993</option>
			<option value="1992">1992</option>
			<option value="1991">1991</option>
			<option value="1990">1990</option>
			<option value="1989">1989</option>
			<option value="1988">1988</option>
			<option value="1987">1987</option>
			<option value="1986">1986</option>
			<option value="1985">1985</option>
			<option value="1984">1984</option>
			<option value="1983">1983</option>
			<option value="1982">1982</option>
			<option value="1981">1981</option>
			<option value="1980">1980</option>
			<option value="1979">1979</option>
			<option value="1978">1978</option>
			<option value="1977">1977</option>
			<option value="1976">1976</option>
			<option value="1975">1975</option>
			<option value="1974">1974</option>
			<option value="1973">1973</option>
			<option value="1972">1972</option>
			<option value="1971">1971</option>
			<option value="1970">1970</option>
			<option value="1969">1969</option>
			<option value="1968">1968</option>
			<option value="1967">1967</option>
			<option value="1966">1966</option>
			<option value="1965">1965</option>
			<option value="1964">1964</option>
			<option value="1963">1963</option>
			<option value="1962">1962</option>
			<option value="1961">1961</option>
			<option value="1960">1960</option>
			<option value="1959">1959</option>
			<option value="1958">1958</option>
			<option value="1957">1957</option>
			<option value="1956">1956</option>
			<option value="1955">1955</option>
			<option value="1954">1954</option>
			<option value="1953">1953</option>
			<option value="1952">1952</option>
			<option value="1951">1951</option>
			<option value="1950">1950</option>
			<option value="1949">1949</option>
			<option value="1948">1948</option>
			<option value="1947">1947</option>
			<option value="1946">1946</option>
			<option value="1945">1945</option>
			<option value="1944">1944</option>
			<option value="1943">1943</option>
			<option value="1942">1942</option>
			<option value="1941">1941</option>
			<option value="1940">1940</option>
			<option value="1939">1939</option>
			<option value="1938">1938</option>
			<option value="1937">1937</option>
			<option value="1936">1936</option>
			<option value="1935">1935</option>
			<option value="1934">1934</option>
			<option value="1933">1933</option>
			<option value="1932">1932</option>
			<option value="1931">1931</option>
			<option value="1930">1930</option>
			<option value="1929">1929</option>
			<option value="1928">1928</option>
			<option value="1927">1927</option>
			<option value="1926">1926</option>
			<option value="1925">1925</option>
			<option value="1924">1924</option>
			<option value="1923">1923</option>
			<option value="1922">1922</option>
			<option value="1921">1921</option>
			<option value="1920">1920</option>
			<option value="1919">1919</option>
			<option value="1918">1918</option>
			<option value="1917">1917</option>
			<option value="1916">1916</option>
			<option value="1915">1915</option>
			<option value="1914">1914</option>
			<option value="1913">1913</option>
			<option value="1912">1912</option>
			<option value="1911">1911</option>
			<option value="1910">1910</option>
			<option value="1909">1909</option>
			<option value="1908">1908</option>
			<option value="1907">1907</option>
			<option value="1906">1906</option>
			<option value="1905">1905</option>
			<option value="1904">1904</option>
			<option value="1903">1903</option>
			<option value="1902">1902</option>
			<option value="1901">1901</option>
			<option value="1900">1900</option>

		</select>
                <img id="TextBoxDateOfBirthCheck" class="RESUinputOkBirth" src="signup.aspx_files/img14x14_Tick.gif" alt="">
                <div id="ctl00_ContentPlaceHolder1_TextBoxDateOfBirthHide" style="display: none;">
                    <a id="AnchorConfirmDateOfBirth" class="btnBlue RESUconfirmBtn" href="javascript:confirmDateOfBirth();"><span id="ctl00_ContentPlaceHolder1_LabelConfirmDateOfBirth">CONFIRM BIRTHDAY</span></a>
                </div>
                <div id="DateOfBirthHint" class="hintContainer RESUhintBirth">
                    <img class="hintContainerArrow" src="signup.aspx_files/img23x17_ArrowBubble.gif" alt="">                                        
                    <p><span id="ctl00_ContentPlaceHolder1_LabelDateOfBirthHint">Knowing your birthday will help us to personalise your LEGO experience. LEGO takes online child safety very seriously.</span></p>
                </div>
                <p class="error RESUerrorBirth">
                    <span id="ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth" style="color: Red; display: none;">Not a valid date.</span>
                    <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth" style="color: Red; display: none;">Please confirm your date of birth!</span>
                </p>
            </div>
            <div id="ctl00_ContentPlaceHolder1_EmailContainer" class="field RESUmailField" style="display: none;">
                <label for="ctl00_ContentPlaceHolder1_TextBoxEMail"><span id="ctl00_ContentPlaceHolder1_LabelEmail_NotChild" class="RESUmailLabel" style="display: none;">E-mail:</span><span id="ctl00_ContentPlaceHolder1_LabelEmail_Child" class="RESUmailChildLabel" style="display: none;">Your Parent's E-mail:</span></label>
                <input name="ctl00$ContentPlaceHolder1$TextBoxEMail" id="ctl00_ContentPlaceHolder1_TextBoxEMail" checkctrlid="TextBoxEMailCheck" hidectrlid="TextBoxEMailHide" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail');hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail');" onblur="ValidatorValidate(ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail);validate(this);" onchange="validate(this);" type="text">
                <img id="TextBoxEMailCheck" class="RESUinputOk" src="signup.aspx_files/img14x14_Tick.gif" alt="">
                <p class="error">
                    <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail" style="color: Red; display: none;">This information is required.</span>
                    <span id="ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail" style="color: Red; display: none;">This is not a valid e-mail address.</span>
                </p>
            </div>
            <div id="PIIContainer" class="field RESUcountryField" style="display: none;">
                <label for="ctl00_ContentPlaceHolder1_DropDownListCountry"><span id="ctl00_ContentPlaceHolder1_LabelCountry" class="RESUcountryLabel">Country:</span></label>
                <select name="ctl00$ContentPlaceHolder1$DropDownListCountry" id="ctl00_ContentPlaceHolder1_DropDownListCountry" checkctrlid="DropDownListCountryCheck" hidectrlid="DropDownListCountryHide" onblur="ValidatorValidate(ctl00_ContentPlaceHolder1_CustomValidatorCountry); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_CustomValidatorCountry);validate(this);" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_CustomValidatorCountry');hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry');" onchange="validate(this);">
			<option selected="selected" value="-1">Please choose...</option>
			<option value="AF">Afghanistan</option>
			<option value="AL">Albania</option>
			<option value="DZ">Algeria</option>
			<option value="AD">Andorra</option>
			<option value="AO">Angola</option>
			<option value="AI">Anguilla</option>
			<option value="AG">Antigua and Barbuda</option>
			<option value="AR">Argentina</option>
			<option value="AM">Armenia</option>
			<option value="AW">Aruba</option>
			<option value="AU">Australia</option>
			<option value="AT">Austria</option>
			<option value="AZ">Azerbaijan</option>
			<option value="BS">Bahamas</option>
			<option value="BH">Bahrain</option>
			<option value="BD">Bangladesh</option>
			<option value="BB">Barbados</option>
			<option value="BY">Belarus</option>
			<option value="BE">Belgium</option>
			<option value="BZ">Belize</option>
			<option value="BJ">Benin</option>
			<option value="BM">Bermuda</option>
			<option value="BT">Bhutan</option>
			<option value="BO">Bolivia</option>
			<option value="BA">Bosnia-Hertzagovina</option>
			<option value="BW">Botswana</option>
			<option value="BR">Brazil</option>
			<option value="VG">British Virgin Islands</option>
			<option value="BN">Brunei Darussalm</option>
			<option value="BG">Bulgaria</option>
			<option value="BF">Burkina Faso</option>
			<option value="MM">Burma (Myanmar)</option>
			<option value="BI">Burundi</option>
			<option value="KH">Cambodia</option>
			<option value="CM">Cameroon</option>
			<option value="CA">Canada</option>
			<option value="CV">Cape Verde</option>
			<option value="KY">Cayman Islands</option>
			<option value="CF">Central African Republic</option>
			<option value="TD">Chad</option>
			<option value="CL">Chile</option>
			<option value="CN">China</option>
			<option value="CX">Christmas Island</option>
			<option value="CC">Cocos Islands</option>
			<option value="CO">Colombia</option>
			<option value="KM">Comoros &amp; Mayotte</option>
			<option value="CG">Congo</option>
			<option value="CK">Cook Islands</option>
			<option value="CR">Costa Rica</option>
			<option value="HR">Croatia</option>
			<option value="CU">Cuba</option>
			<option value="CY">Cyprus</option>
			<option value="CZ">Czech Republic</option>
			<option value="DK">Denmark</option>
			<option value="DJ">Djibouti</option>
			<option value="DM">Dominica</option>
			<option value="DO">Dominican Republic</option>
			<option value="TP">East Timor</option>
			<option value="EC">Ecuador</option>
			<option value="EG">Egypt</option>
			<option value="SV">El Salvador</option>
			<option value="GQ">Equatorial Guinea</option>
			<option value="EE">Estonia</option>
			<option value="ET">Ethiopia</option>
			<option value="FK">Falkland Islands</option>
			<option value="FO">Faroe Islands</option>
			<option value="FJ">Fiji Islands</option>
			<option value="FI">Finland</option>
			<option value="FR">France</option>
			<option value="GF">French Guiana</option>
			<option value="PF">French Polynesia</option>
			<option value="GA">Gabonese Republic</option>
			<option value="GM">Gambia</option>
			<option value="GE">Georgia</option>
			<option value="DE">Germany</option>
			<option value="GH">Ghana</option>
			<option value="GI">Gibraltar</option>
			<option value="GR">Greece</option>
			<option value="GL">Greenland</option>
			<option value="GD">Grenada/Carricou</option>
			<option value="GP">Guadeloupe</option>
			<option value="GU">Guam</option>
			<option value="GT">Guatemala</option>
			<option value="GN">Guinea</option>
			<option value="GW">Guinea-Bissau</option>
			<option value="GY">Guyana</option>
			<option value="HT">Haiti</option>
			<option value="HN">Honduras</option>
			<option value="HK">Hong Kong</option>
			<option value="HU">Hungary</option>
			<option value="IS">Iceland</option>
			<option value="IN">India</option>
			<option value="ID">Indonesia</option>
			<option value="IR">Iran</option>
			<option value="IQ">Iraq</option>
			<option value="IE">Ireland</option>
			<option value="IL">Israel</option>
			<option value="IT">Italy</option>
			<option value="CI">Ivory Coast</option>
			<option value="JM">Jamaica</option>
			<option value="JP">Japan</option>
			<option value="JO">Jordan</option>
			<option value="KZ">Kazakstan</option>
			<option value="KE">Kenya</option>
			<option value="KI">Kiribati, Gilbert Islands</option>
			<option value="KW">Kuwait</option>
			<option value="KG">Kyrgyz Republic</option>
			<option value="LA">Laos</option>
			<option value="LV">Latvia</option>
			<option value="LB">Lebanon</option>
			<option value="LS">Lesotho</option>
			<option value="LR">Liberia</option>
			<option value="LY">Libya</option>
			<option value="LI">Liechtenstein</option>
			<option value="LT">Lithuania</option>
			<option value="LU">Luxembourg</option>
			<option value="MO">Macao</option>
			<option value="MK">Macedonia</option>
			<option value="MG">Madagascar</option>
			<option value="MW">Malawi</option>
			<option value="MY">Malaysia</option>
			<option value="MV">Maldives</option>
			<option value="ML">Mali</option>
			<option value="MT">Malta</option>
			<option value="MQ">Martinique</option>
			<option value="MR">Mauritania</option>
			<option value="MU">Mauritius</option>
			<option value="MX">Mexico</option>
			<option value="FM">Micronesia</option>
			<option value="MD">Moldova</option>
			<option value="MC">Monaco</option>
			<option value="MN">Mongolia</option>
			<option value="MS">Montserrat</option>
			<option value="MA">Morocco</option>
			<option value="MZ">Mozambique</option>
			<option value="MP">N. Mariana Islands</option>
			<option value="NA">Namibia</option>
			<option value="NR">Nauru</option>
			<option value="NP">Nepal</option>
			<option value="NL">Netherlands</option>
			<option value="AN">Netherlands Antilles</option>
			<option value="NC">New Caledonia</option>
			<option value="NZ">New Zealand</option>
			<option value="NI">Nicaragua</option>
			<option value="NE">Niger</option>
			<option value="NG">Nigeria</option>
			<option value="NU">Niue Island</option>
			<option value="KP">North Korea</option>
			<option value="ND">Northern Ireland</option>
			<option value="NO">Norway</option>
			<option value="OM">Oman</option>
			<option value="PK">Pakistan</option>
			<option value="PW">Palau</option>
			<option value="PA">Panama</option>
			<option value="PG">Papua New Guinea</option>
			<option value="PY">Paraguay</option>
			<option value="PE">Peru</option>
			<option value="PH">Philippines</option>
			<option value="PN">Pitcairn Islands</option>
			<option value="PL">Poland</option>
			<option value="PT">Portugal</option>
			<option value="PR">Puerto Rico</option>
			<option value="QA">Qatar</option>
			<option value="RE">Reunion (France)</option>
			<option value="RO">Romania</option>
			<option value="RU">Russia</option>
			<option value="RW">Rwanda</option>
			<option value="SM">San Marino</option>
			<option value="ST">Sao Tome &amp; Principe</option>
			<option value="SA">Saudi Arabia</option>
			<option value="SN">Senegal</option>
			<option value="YU">Serbia and Montenegro</option>
			<option value="SC">Seychelles</option>
			<option value="SL">Sierra Leone</option>
			<option value="SG">Singapore</option>
			<option value="SK">Slovakia</option>
			<option value="SI">Slovenia</option>
			<option value="SB">Solomon Islands</option>
			<option value="SO">Somalia</option>
			<option value="ZA">South Africa</option>
			<option value="KR">South Korea</option>
			<option value="ES">Spain</option>
			<option value="LK">Sri Lanka</option>
			<option value="SH">St Helena</option>
			<option value="KN">St Kitts and Nevis</option>
			<option value="LC">St Lucia</option>
			<option value="PM">St Pierre &amp; Miquelon</option>
			<option value="VC">St Vincent/Grenadines</option>
			<option value="SD">Sudan</option>
			<option value="SR">Suriname</option>
			<option value="SZ">Swaziland</option>
			<option value="SE">Sweden</option>
			<option value="CH">Switzerland</option>
			<option value="SY">Syria</option>
			<option value="TW">Taiwan</option>
			<option value="TJ">Tajikistan</option>
			<option value="TZ">Tanzania</option>
			<option value="TH">Thailand</option>
			<option value="TG">Togolese Republic</option>
			<option value="TO">Tonga</option>
			<option value="TT">Trinidad and Tobago</option>
			<option value="TN">Tunisia</option>
			<option value="TR">Turkey</option>
			<option value="TM">Turkmenistan</option>
			<option value="TC">Turks and Caicos</option>
			<option value="TV">Tuvalu, Ellice Islands</option>
			<option value="UG">Uganda</option>
			<option value="UA">Ukraine</option>
			<option value="AE">United Arab Emirates</option>
			<option value="UK">United Kingdom</option>
			<option value="US">United States</option>
			<option value="UY">Uruguay</option>
			<option value="UZ">Uzbekistan</option>
			<option value="VU">Vanuatu</option>
			<option value="VA">Vatican City</option>
			<option value="VE">Venezuela</option>
			<option value="VN">Vietnam</option>
			<option value="WS">Western Samoa</option>
			<option value="YE">Yemen</option>
			<option value="ZR">Zaire</option>
			<option value="ZM">Zambia</option>
			<option value="ZW">Zimbabwe</option>

		</select>
                <p class="error">
                    <span id="ctl00_ContentPlaceHolder1_CustomValidatorCountry" style="color: Red; display: none;">To sign up for a LEGO ID in South Korea, <a href="https://account.lego.com/ko-kr/registration/flash.aspx?ReturnUrl=http://universe.lego.com/en-us/betacenter/default.aspx">click here.</a></span>
                    <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry" style="color: Red; display: none;">This information is required.</span>     
                </p>
                <div id="genderField">
                    <span id="ctl00_ContentPlaceHolder1_LabelGender" class="nonInputLabel RESUgenderLabel">Gender:</span>
                    <table id="ctl00_ContentPlaceHolder1_RadioButtonListGender" checkctrlid="RadioButtonListGenderCheck" hidectrlid="RadioButtonListGenderHide" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender');" onchange="validate(this);" border="0">
			<tbody><tr>
				<td><input id="ctl00_ContentPlaceHolder1_RadioButtonListGender_0" name="ctl00$ContentPlaceHolder1$RadioButtonListGender" value="M" type="radio"><label for="ctl00_ContentPlaceHolder1_RadioButtonListGender_0">Male</label></td><td><input id="ctl00_ContentPlaceHolder1_RadioButtonListGender_1" name="ctl00$ContentPlaceHolder1$RadioButtonListGender" value="F" type="radio"><label for="ctl00_ContentPlaceHolder1_RadioButtonListGender_1">Female</label></td>
			</tr>
		</tbody></table>
                    <p class="error">
                        <span id="ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender" style="color: Red; display: none;">This information is required.</span>
                    </p>
                </div>
            </div>
            <div class="seperator RESUseperatorLast"></div>
            <div class="field">
              <input type="submit"  value="SUBMIT" id="ctl00_ContentPlaceHolder1_ImageButtonRegister" class="RESUsubmitBtn btnGreen" />
                    </div>
        </fieldset>
        <img class="RESUminiFig" src="signup.aspx_files/img185x285_Minifig.gif" alt=""><br>
        <div id="footerSmall" class="REfooter">
            <a href="javascript:showFAQ();">LEGO ID HELP</a>
        </div>
        <input name="ctl00$ContentPlaceHolder1$TextBoxDateOfBirth" id="ctl00_ContentPlaceHolder1_TextBoxDateOfBirth" checkctrlid="TextBoxDateOfBirthCheck" hidectrlid="TextBoxDateOfBirthHide" onblur="ValidatorValidate(ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth); ValidatorUpdateDisplay(ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth);validate(this);" onfocus="hideError(this,'ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth');hideTick(this);hideError(this,'ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth');" onchange="validate(this);" style="height: 0px; width: 0px; visibility: hidden;" type="text">    
    </div>
    
	</div>

            </div>
      </div>
        
    
<script type="text/javascript">
//<![CDATA[
var Page_Validators =  new Array(document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername"), document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername"), document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorUsername"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword"), document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername"), document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorPassword"), document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword"), document.getElementById("ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword"), document.getElementById("ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail"), document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail"), document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorCountry"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry"), document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender"));
	//]]>
</script>

<script type="text/javascript">
//<![CDATA[
var ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords");
	ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUserName";
	ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords.evaluationfunction = "CustomValidatorEvaluateIsValid";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUserName";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername.initialvalue = "";
	var ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername");
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUserName";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername.errormessage = "The username contains invalid characters.";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername.evaluationfunction = "RegularExpressionValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername.validationexpression = "^([A-Za-z_-]{1}[A-Za-z0-9_-]{1,30})|([A-Za-z0-9_-]{1,30}[A-Za-z_-]{1})|([A-Za-z0-9_-]{1,30}[A-Za-z_-]{1}[A-Za-z0-9_-]{1,30})$";
	var ctl00_ContentPlaceHolder1_CustomValidatorUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorUsername");
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxUserName";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.errormessage = "The username contains more than 4 numbers.";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.evaluationfunction = "CustomValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_CustomValidatorUsername.clientvalidationfunction = "customUsernameValidator";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword.initialvalue = "";
	var ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername");
	ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername.errormessage = "The password cannot be the same as the username.";
	ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername.evaluationfunction = "CustomValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername.clientvalidationfunction = "customPasswordValidator";
	var ctl00_ContentPlaceHolder1_CustomValidatorPassword = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorPassword"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorPassword");
	ctl00_ContentPlaceHolder1_CustomValidatorPassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_CustomValidatorPassword.errormessage = "Your password is not safe. Please choose another one.";
	ctl00_ContentPlaceHolder1_CustomValidatorPassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorPassword.evaluationfunction = "CustomValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_CustomValidatorPassword.clientvalidationfunction = "customPasswordInvalidValidator";
	var ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword = document.all ? document.all["ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword"] : document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword");
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword.errormessage = "Your password is not safe. Please choose another one.";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword.evaluationfunction = "RegularExpressionValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword.validationexpression = ".{6,40}";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxRetypePassword";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword.initialvalue = "";
	var ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword = document.all ? document.all["ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword"] : document.getElementById("ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword");
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxRetypePassword";
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.errormessage = "Please verify your password again!";
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.evaluationfunction = "CompareValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.controltocompare = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword.controlhookup = "ctl00_ContentPlaceHolder1_TextBoxPassword";
	var ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth = document.all ? document.all["ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth"] : document.getElementById("ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth");
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxDateOfBirth";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.errormessage = "Not a valid date.";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.type = "Date";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.dateorder = "mdy";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.cutoffyear = "2029";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.century = "2000";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.evaluationfunction = "RangeValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.maximumvalue = "2010/09/05";
	ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth.minimumvalue = "0001/01/01";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxDateOfBirth";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth.errormessage = "Please confirm your date of birth!";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth.initialvalue = "";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxEMail";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail.initialvalue = "";
	var ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail = document.all ? document.all["ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail"] : document.getElementById("ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail");
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail.controltovalidate = "ctl00_ContentPlaceHolder1_TextBoxEMail";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail.errormessage = "This is not a valid e-mail address.";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail.evaluationfunction = "RegularExpressionValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail.validationexpression = "[A-Za-z0-9!#$%&\'*+/=?^_`{|}~-]+(?:\\.[A-Za-z0-9!#$%&\'*+/=?^_`{|}~-]+)*@(?:[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?\\.)+[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?";
	var ctl00_ContentPlaceHolder1_CustomValidatorCountry = document.all ? document.all["ctl00_ContentPlaceHolder1_CustomValidatorCountry"] : document.getElementById("ctl00_ContentPlaceHolder1_CustomValidatorCountry");
	ctl00_ContentPlaceHolder1_CustomValidatorCountry.controltovalidate = "ctl00_ContentPlaceHolder1_DropDownListCountry";
	ctl00_ContentPlaceHolder1_CustomValidatorCountry.errormessage = "To sign up for a LEGO ID in South Korea, <a href=\'https://account.lego.com/ko-kr/registration/flash.aspx?ReturnUrl=http://universe.lego.com/en-us/betacenter/default.aspx\'>click here.</a>";
	ctl00_ContentPlaceHolder1_CustomValidatorCountry.display = "Dynamic";
	ctl00_ContentPlaceHolder1_CustomValidatorCountry.evaluationfunction = "CustomValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_CustomValidatorCountry.clientvalidationfunction = "validateCountrySelection";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.controltovalidate = "ctl00_ContentPlaceHolder1_DropDownListCountry";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry.initialvalue = "-1";
	var ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender = document.all ? document.all["ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender"] : document.getElementById("ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender");
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.controltovalidate = "ctl00_ContentPlaceHolder1_RadioButtonListGender";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.errormessage = "This information is required.";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.display = "Dynamic";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.enabled = "False";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.evaluationfunction = "RequiredFieldValidatorEvaluateIsValid";
	ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender.initialvalue = "";
	//]]>
</script>

	
<script type="text/javascript">
//<![CDATA[
addClickFunction('ctl00_ContentPlaceHolder1_ImageButtonRegister'); var invalidPwd=',AAAAAA,123456,PASSWORD,QWERTY,'
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
        WebForm_AutoFocus('ctl00_ContentPlaceHolder1_TextBoxUserName');Sys.Application.initialize();

document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorInvalidWords'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorUsername'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorUsername'));
}

document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorUsername').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorUsername'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorPassword'));
}

document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorPasswordUsername'));
}

document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorPassword').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorPassword'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorPassword'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorRetypePassword'));
}

document.getElementById('ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CompareValidatorPasswordAndRetypePassword'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RangeValidatorDateOfBirth'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorDateOfBirth'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorEMail'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RegularExpressionValidatorEMail'));
}

document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorCountry').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_CustomValidatorCountry'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorCountry'));
}

document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender').dispose = function() {
    Array.remove(Page_Validators, document.getElementById('ctl00_ContentPlaceHolder1_RequiredFieldValidatorGender'));
}
//]]>
</script>
</form>
</body></html>