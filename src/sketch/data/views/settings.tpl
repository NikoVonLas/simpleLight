<nav class="navigation">
    <section class="container">
        <h1 class="title">Настройки WiFi</h1>
        <ul class="navigation-list float-right">
            <li class="navigation-item"><a class="navigation-link" href="#" id="to-home" data-popover="">Назад</a></li>
        </ul>
    </section>
</nav>
<div class="container main">
    <form id="settings" name="settings" method="POST">
        <div class="row">
            <fieldset class="center">
                <div class="field-group">
                    <label for="num_leds">Количество светодиодов в ленте</label>
                    <input id="num_leds" name="num_leds" type="number" min="1" max="5000" step="1" value="{{=it.num_leds}}">
                </div>
                <div class="field-group">
                    <label for="ssid">Название точки доступа</label>
                    <select id="ssid" name="ssid">
                        <option selected value="{{=it.ssid}}">{{=it.ssid}}</option>
                        {{~it.ssids :value:index}}
                            <option value="{{=value}}">{{=value}}</option>
                        {{~}}
                    </select>
                </div>
                <div class="field-group">
                    <label for="pass">Пароль</label>
                    <input id="pass" name="pass" type="password" size="32" value="{{=it.pass}}">
                </div>
                <div class="button-container">
                    <button type="submit" class="button">Сохранить</button>
                </div>
            </fieldset>
        </div>
    </form>
    <form method='POST' action='/update' enctype='multipart/form-data' class="second-form">
        <div class="row">
            <fieldset class="center">
                <div class="field-group">
                    <label class="file">
                        <input type="file" name='spiffs' id="spiffs" accept="spiffs.bin">
                        <span id="spiffs-custom" class="file-custom spiffs"></span>
                    </label>
                </div>
                <div class="field-group">
                    <label class="file">
                    <label class="file">
                        <input type="file" name='update' id="update" accept=".bin">
                        <span id="update-custom" class="file-custom update"></span>
                    </label>
                </div>
                <div class="button-container">
                    <button type='submit' class="button">Обновить прошивку</button>
                </div>
            </fieldset>
        </div>
    </form>
</div>