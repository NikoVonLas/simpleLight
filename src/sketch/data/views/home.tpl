<nav class="navigation">
   <section class="container">
      <img class="logo" src="/assets/img/icon48.png" alt="Лого">
      <h1 class="title">SimpleLight</h1>
      <ul class="navigation-list float-right">
         <li class="navigation-item"><a class="navigation-link" href="#" id="to-settings" data-popover="">Настройки</a></li>
      </ul>
      <a href="https://github.com/NikoVonLas/simpleLight" rel="noopener" target="_blank" title="SimpleLight на Github">
         <svg class="octocat" viewBox="0 0 250 250">
            <path d="M0,0 L115,115 L130,115 L142,142 L250,250 L250,0 Z"></path>
            <path class="octocat-arm" d="M128.3,109.0 C113.8,99.7 119.0,89.6 119.0,89.6 C122.0,82.7 120.5,78.6 120.5,78.6 C119.2,72.0 123.4,76.3 123.4,76.3 C127.3,80.9 125.5,87.3 125.5,87.3 C122.9,97.6 130.6,101.9 134.4,103.2"></path>
            <path class="octocat-body" d="M115.0,115.0 C114.9,115.1 118.7,116.5 119.8,115.4 L133.7,101.6 C136.9,99.2 139.9,98.4 142.2,98.6 C133.8,88.0 127.5,74.4 143.8,58.0 C148.5,53.4 154.0,51.2 159.7,51.0 C160.3,49.4 163.2,43.6 171.4,40.1 C171.4,40.1 176.1,42.5 178.8,56.2 C183.1,58.6 187.2,61.8 190.9,65.4 C194.5,69.0 197.7,73.2 200.1,77.6 C213.8,80.2 216.3,84.9 216.3,84.9 C212.7,93.1 206.9,96.0 205.4,96.6 C205.1,102.4 203.0,107.8 198.3,112.5 C181.9,128.9 168.3,122.5 157.7,114.1 C157.9,116.9 156.7,120.9 152.7,124.9 L141.0,136.5 C139.8,137.7 141.6,141.9 141.8,141.8 Z"></path>
         </svg>
      </a>
   </section>
</nav>
<div class="container main">
   <form name="settings" method="POST">
      <div class="row">
         <div id="column-first" class="column">
            <fieldset>
            	<label for="mode">Режим</label>
            	<select name="mode" id="mode" form="settings">
            		<option value="0"{{? it.this_mode==0}}selected {{?}}>Белый</option>
            		<option value="1"{{? it.this_mode==1}}selected {{?}}>Постоянный цвет</option>
            		<option value="2"{{? it.this_mode==2}}selected {{?}}>Плавная смена цвета</option>
            		<option value="3"{{? it.this_mode==3}}selected {{?}}>Бегущая радуга</option>
                  <option value="4"{{? it.this_mode==4}}selected {{?}}>Огонь</option>
            	</select>
            	<label for="brightness">Общая яркость</label>
               <div id="slider-container-brightness" data-input="brightness"></div>
            	<input name="brightness" id="brightness" data-elem="slider-container-brightness" data-plugin="range" type="number" min="1" max="255" step="1" form="settings" value="{{=it.brightness}}">
            </fieldset>
         </div>
         <div id="column-second" class="column">
            <fieldset>
            	<label for="first_param">
            	{{? it.this_mode==1}}
            		Цвет
            	{{?? it.this_mode==2}}
            		Скорость смены цвета
            	{{?? it.this_mode==3}}
            		Скорость радуги
               {{?? it.this_mode==4}}
                  Цвет
            	{{?}}
            	</label>
            	{{? it.this_mode==1}}
                  <div id="color-container-light_color"></div>
            		<input name="light_color" id="first_param" data-saturation="second_param" data-elem="color-container-light_color" data-plugin="color" type="number" min="1" max="360" step="1" form="settings" value="{{=it.light_color}}">
            	{{?? it.this_mode==2}}
                  <div class="range-margin" id="slider-container-color_speed" data-input="first_param"></div>
            		<input name="color_speed" id="first_param" data-elem="slider-container-color_speed" data-plugin="range" type="number" min="1" max="255" step="1" form="settings" value="{{=it.color_speed}}">
            	{{?? it.this_mode==3}}
                  <div class="range-margin" id="slider-container-rainbow_period" data-input="first_param"></div>
            		<input name="rainbow_period" id="first_param" data-elem="slider-container-rainbow_period" data-plugin="range" type="number" min="1" max="40" step="1" form="settings" value="{{=it.rainbow_period}}">
               {{?? it.this_mode==4}}
                  <div class="range-margin" id="color-container-fire_color" data-input="first_param"></div>
                  <input name="fire_color" id="first_param" data-elem="color-container-fire_color" data-plugin="color" type="number" min="1" max="360" step="1" form="settings" value="{{=it.fire_color}}">
            	{{?}}
            	<label class="label-margin" for="second_param">
            	{{? it.this_mode==2}}
            		Насыщенность
            	{{?? it.this_mode==3}}
            		Шаг радуги
               {{?? it.this_mode==4}}
                  Интенсивность
            	{{?}}
            	</label>
            	{{? it.this_mode==1}}
            		<input name="light_sat" id="second_param" type="number" min="1" max="100" form="settings" value="{{=it.light_sat}}">
               {{?? it.this_mode==2}}
                  <div id="slider-container-light_sat" data-input="second_param"></div>
                  <input name="light_sat" id="second_param" data-elem="slider-container-light_sat" data-plugin="range" type="number" min="1" max="100" step="1" form="settings" value="{{=it.light_sat}}">
            	{{?? it.this_mode==3}}
                  <div id="slider-container-rainbow_step" data-input="second_param"></div>
            		<input name="rainbow_step" id="second_param" data-elem="slider-container-rainbow_step" data-plugin="range" type="number" min="0.1" max="10" step="0.1" form="settings" value="{{=it.rainbow_step}}">
               {{?? it.this_mode==4}}
                  <div id="slider-container-fire_speed" data-input="second_param"></div>
                  <input name="fire_speed" id="second_param" data-elem="slider-container-fire_speed" data-plugin="range" type="number" min="1" max="100" step="1" form="settings" value="{{=it.fire_speed}}">
            	{{?}}
            </fieldset>
         </div>
      </div>
      <div class="row">
        <div class="column">
        {{? it.onstate}}
         	<a href="#" onclick="off();" class="button">Отключить</a>
        {{??}}
         	<a href="#" onclick="on();" class="button">Включить</a>
        {{?}}
     	</div>
      </div>
   </form>
</div>