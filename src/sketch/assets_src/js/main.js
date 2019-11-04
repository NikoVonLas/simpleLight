var UID = {
    _current: 0,
    getNew: function(){
        this._current++;
        return this._current;
    }
};

HTMLElement.prototype.pseudoStyle = function(element,prop,value){
    var _this = this;
    var _sheetId = 'pseudoStyles';
    var _head = document.head || document.getElementsByTagName('head')[0];
    var _sheet = document.getElementById(_sheetId) || document.createElement('style');
    _sheet.id = _sheetId;
    var className = 'pseudoStyle' + UID.getNew();
    
    _this.className +=  ' '+className; 
    
    _sheet.innerHTML += ' .'+className+':'+element+'{'+prop+':'+value+'}';
    _head.appendChild(_sheet);
    return this;
};

function sendData(e) {
    var n, t = new XMLHttpRequest,
        o = [];
    o.push(encodeURIComponent(e.name) + '=' + encodeURIComponent(e.value));
    n = o.join('&').replace(/%20/g, '+');
    t.addEventListener('load', function(e) {
        if ('reload' == t.response) document.location.reload();
        if ('reboot' == t.response) {
            setTimeout(function() {
                document.location.reload()
            }, 5000);
            alert('SimpleLight перезагружается');
        }
        console.log('Yeah! Data sent and response loaded.');
    });
    t.addEventListener('error', function(e) {
        console.log('Oops! Something goes wrong.')
    });
    t.open('POST', '/settings');
    t.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    t.send(n);
}

function on() {
    sendData({name: 'state', value: 'on'});
}

function off() {
    sendData({name: 'state', value: 'off'});
}

function getHomeTpl() {
    var e;
    window.XMLHttpRequest && (e = new XMLHttpRequest), e.onreadystatechange = function() {
        4 == e.readyState && 200 == e.status && setData(e.responseText, 'home')
    }, e.open('GET', '/views/home.tpl', !0), e.send()
}

function getSettingsTpl() {
    var e;
    window.XMLHttpRequest && (e = new XMLHttpRequest), e.onreadystatechange = function() {
        4 == e.readyState && 200 == e.status && setData(e.responseText, 'settings')
    }, e.open('GET', '/views/settings.tpl', !0), e.send()
}

function setData(n, page) {
    var t;
    n = n || document.getElementById('container').innerHTML;
    window.XMLHttpRequest && (t = new XMLHttpRequest), t.onreadystatechange = function() {
        if (4 == t.readyState && 200 == t.status) {
            var e = doT.template(n)(JSON.parse(t.responseText));
            document.getElementById('container').innerHTML = e;
            if (page == 'home') {
                setHandlers();
                document.getElementById('to-settings').onclick = function() {
                    getSettingsTpl()
                };
            } else {
                setHandlersSettings();
                document.getElementById('to-home').onclick = function() {
                    getHomeTpl()
                };
            }
        }
    }, t.open('GET', '/info.json', !0), t.send()
}

function setHandlers() {
    inputs = document.getElementsByTagName('input');
    for (var e = 0; e < inputs.length; e++) {
        if (inputs[e].getAttribute('type') == 'number' && inputs[e].getAttribute('data-plugin') == 'range') {
            elem = document.getElementById(inputs[e].getAttribute('data-elem'));
            noUiSlider.create(elem, {
                start: [inputs[e].value],
                step: parseInt(inputs[e].getAttribute('step')),
                range: {
                    min: parseInt(inputs[e].getAttribute('min')),
                    max: parseInt(inputs[e].getAttribute('max'))
                },
                format: wNumb({
                    decimals: (parseInt(inputs[e].getAttribute('step')) < 0) ? 2 : 0
                })
            });
            elem.noUiSlider.on('set', function (values, handle) {
                el = document.getElementById(this.target.getAttribute('data-input'));
                el.value = values[handle];
                el.onchange();
            });
        } else if (inputs[e].getAttribute('type') == 'number' && inputs[e].getAttribute('data-plugin') == 'color') {
            var colorInput = inputs[e];
            if (typeof inputs[e].getAttribute('data-saturation') != 'undefined' && inputs[e].getAttribute('data-saturation') != null) {
                var satInput = document.getElementById(inputs[e].getAttribute('data-saturation'));
            }
            width = document.getElementById('column-second').offsetWidth;
            colorPicker = new iro.ColorPicker('#' + inputs[e].getAttribute('data-elem'), {
                width: width,
                color: {h: inputs[e].value, s: ((typeof satInput != 'undefined' && satInput != null) ? satInput.value : 100), v: 100},
                borderWidth: 3,
                borderColor: '#9b4dca',
                layout: [
                {
                  component: iro.ui.Wheel,
                  options: {}
                },
              ]
            });
            colorPicker.on('input:end', function (color) {
                colorInput.value = color.hsv.h;
                colorInput.onchange();
                if (typeof satInput != 'undefined') {
                    satInput.value = color.hsv.s;
                    satInput.onchange();
                }
            });
        }

        inputs[e].onchange = function() {
            sendData(this);
        };
    }
    selects = document.getElementsByTagName('select');
    for (e = 0; e < selects.length; e++) selects[e].onchange = function() {
        sendData(this);
    };
}

function setHandlersSettings() {
    form = document.getElementById('settings');
    form.onsubmit = function(evt) {
        evt.preventDefault();

        ssid = encodeURIComponent(document.getElementById('ssid').value);
        pass = encodeURIComponent(document.getElementById('pass').value);
        numLeds = encodeURIComponent(document.getElementById('num_leds').value);

        var n, t = new XMLHttpRequest,
            o = [];
        o.push(encodeURIComponent('ssid') + '=' + encodeURIComponent(ssid));
        o.push(encodeURIComponent('pass') + '=' + encodeURIComponent(pass));
        o.push(encodeURIComponent('num_leds') + '=' + encodeURIComponent(numLeds));
        n = o.join('&').replace(/%20/g, '+');
        t.addEventListener('load', function(e) {
            if ('reload' == t.response) document.location.reload();
            if ('reboot' == t.response) {
                setTimeout(function() {
                    document.location.reload()
                }, 5000);
                alert('SimpleLight перезагружается');
            }
            console.log('Yeah! Data sent and response loaded.');
        });
        t.addEventListener('error', function(e) {
            console.log('Oops! Something goes wrong.');
        });
        t.open('POST', '/settings');
        t.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        t.send(n);
    };
    file = document.getElementById('update');
    file.onchange = function() {
        document.getElementById('update-custom').pseudoStyle('after', 'content', '"' + this.files[0].name + '"!important');
    };
    spiffs = document.getElementById('spiffs');
    spiffs.onchange = function() {
        document.getElementById('spiffs-custom').pseudoStyle('after', 'content', '"' + this.files[0].name + '"!important');
    };
}

document.addEventListener("DOMContentLoaded", function() {
    getHomeTpl();
});