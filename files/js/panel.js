const menu=document.getElementById('menu');
const btn_menu=document.getElementById('nav_menu');
const btn_sys=document.getElementById('nav_sys');
const btn_sens=document.getElementById('nav_sens');
const btn_prot=document.getElementById('nav_prot');
const btn_net=document.getElementById('nav_net');
const btn_dash=document.getElementById('nav_dash');
const div_main=document.getElementById('main');

const btn_test=document.getElementById('test'); //Borrar

btn_menu.addEventListener('click', () => {
    menu.classList.toggle('is-hidden');
});

btn_sys.addEventListener('click', () => {

    menu.innerHTML=
    `
    <p class="menu-label is-size-5">
    System
  </p>
  <ul class="menu-list is-size-5">
    <li><a>Login</a></li>
    <li><a>Node</a></li>
    <li><a></a></li>
  </ul>
    `
});

btn_dash.addEventListener('click', () => {
    //alert('Hola')
    menu.innerHTML=
    `
    <p class="menu-label is-size-5">
    Dashboard
  </p>
  <ul class="menu-list is-size-5">
    <li><a>View</a></li>
    <li><a>Settings</a></li>
  </ul>
    `
});

btn_sens.addEventListener('click', () => {
    menu.innerHTML=
    `
    <p class="menu-label is-size-5">
    Sensors
  </p>
  <ul class="menu-list is-size-5">
    <li><a>Wired</a></li>
    <li><a>Battery</a></li>
    <li><a>Wireless</a></li>
  </ul>
    `
});

btn_prot.addEventListener('click', () => {
    //alert('Hola')
    menu.innerHTML=
    `
    <p class="menu-label is-size-5">
    Protocols
  </p>
  <ul class="menu-list is-size-5">
    <li><a>MQTT</a></li>
    <li><a>Websocket Client</a></li>
    <li><a>Websocket Server</a></li>
  </ul>
    `
});

btn_net.addEventListener('click', () => {
    //alert('Hola')
    menu.innerHTML=
    `
    <p class="menu-label is-size-5">
    Networks
  </p>
  <ul class="menu-list is-size-5">
    <li><a>Ethernet</a></li>
    <li><a>Station</a></li>
    <li><a>Acces Point</a></li>
  </ul>
    `
});


btn_test.addEventListener('click', ()=>{

    toggle();
    
    div_main.innerHTML=`


    ip_info;
    dns[3];
    dhcp;
    ip6_addr;
    ip6_type;
    dns6[3];

</div>
    `
});
