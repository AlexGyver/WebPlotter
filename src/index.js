import UI from '@alexgyver/ui';
import { EL } from '@alexgyver/component';
import SVPlot from '@alexgyver/svplot';
import SerialJS from '@alexgyver/serial';
import WebSocketJS from '@alexgyver/websocket';
import BLEJS from '@alexgyver/ble';
import './index.css'

if ('serviceWorker' in navigator && typeof USE_SW !== 'undefined') {
    navigator.serviceWorker.register('sw.js');
}

/** @type {SVPlot} */
let plot;

/** @type {UI} */
let ui;

let serial = new SerialJS();
let ws = new WebSocketJS();
let ble = new BLEJS();

let help = `data: $val1,val2\\n
log: @text\\n
label: #label,label\\n
unit: #label[%]\\n
dots: #--label\\n`;

document.addEventListener("DOMContentLoaded", () => {
    let sidebar = EL.make('div', {
        parent: document.body,
        class: 'sidebar',
    });
    let container = EL.make('div', {
        parent: document.body,
        class: 'container',
    });

    plot = new SVPlot(container, { type: 'stack', /*dark: true*/ });

    ui = new UI({ parent: sidebar, /*theme: 'dark noback',*/ width: '100%' })
        .addSelect('conn', 'Connection', ['Serial', 'BLE', 'WS'], connsel_h)
        .addButton('select', 'Select', select_h)
        .addSpace()
        .addLabel('name', 'Name', 'none')
        .addLabel('state', 'State', 'none')
        .addNumber('baud', 'Baudrate', 115200)
        .addText('ws', 'IP:port', '192.168.1.2:81')
        .addButtons({ connect: ['Connect', conn_h], disconnect: ['Disconnect', disc_h] })
        .addSpace()
        .addArea('log', 'Log', help)
        .addButton('clear', 'Clear', clear_h)
        .addSpace()
        .addText('text', 'Send')
        .addButton('send', 'Send', send_h)

    serial.onchange = onstate;
    ble.onchange = onstate;
    ws.onchange = onstate;

    serial.onselect = name => {
        if (ui.connText == 'Serial') ui.name = name;
    }
    ble.onselect = name => {
        if (ui.connText == 'BLE') ui.name = name;
    }

    serial.ontext = parse_h;
    ble.ontext = parse_h;
    ws.ontext = parse_h;

    let ls_ws = localStorage.getItem('plot_ws');
    if (ls_ws) ui.ws = ls_ws;

    let ls_conn = localStorage.getItem('plot_conn');
    if (ls_conn) ui.conn = ls_conn;

    connsel_h();
});

function onstate(state) {
    ui.state = state;
}

function conn_h() {
    switch (ui.connText) {
        case 'Serial':
            serial.config({ baud: ui.baud });
            serial.open();
            break;

        case 'BLE':
            ble.open();
            break;

        case 'WS': {
            const [ip, port] = ui.ws.split(':');
            ws.config({ ip, port });
            ws.open();
            localStorage.setItem('plot_ws', ui.ws);
        } break;
    }
}

function select_h() {
    switch (ui.connText) {
        case 'Serial': serial.select(); break;
        case 'BLE': ble.select(); break;
    }
}

function connsel_h() {
    disc_h();

    ui.widget('select').hide();
    ui.widget('baud').hide();
    ui.widget('name').hide();
    ui.widget('ws').hide();

    switch (ui.connText) {
        case 'Serial':
            ui.widget('baud').show();
            ui.widget('select').show();
            ui.widget('name').show();
            ui.name = serial.getName();
            break;

        case 'BLE':
            ui.widget('select').show();
            ui.widget('name').show();
            ui.name = ble.getName();
            break;

        case 'WS':
            ui.widget('ws').show();
            break;
    }

    localStorage.setItem('plot_conn', ui.conn);
}

function disc_h() {
    serial.close();
    ble.close();
    ws.close();
}

function parse_h(str) {
    let cmd = str[0];
    str = str.slice(1);

    switch (cmd) {
        case '$':
            plot.setData(str.split(','));
            break;

        case '#':
            plot.setLabels(str.split(','));
            break;

        case '@': {
            ui.log += (ui.log ? '\n' : '') + str;
            const area = ui.widget('log').input;
            area.scrollTop = area.scrollHeight;
        } break;
    }
}

function clear_h() {
    ui.log = '';
}

function send_h() {
    switch (ui.connText) {
        case 'Serial': serial.sendText(ui.text); break;
        case 'BLE': ble.sendText(ui.text); break;
        case 'WS': ws.sendText(ui.text); break;
    }
    ui.text = '';
}
