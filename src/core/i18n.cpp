#include <i18n.h>
#include "config.h"

// English strings
static const char* const strings_en[] = {
    // Main Menu Items
    "WiFi",
    "BLE",
    "RF",
    "IR",
    "RFID",
    "NRF24",
    "FM",
    "Files",
    "GPS",
    "JS Interpreter",
    "Others",
    "Clock",
    "Connect",
    "Config",
    
    // WiFi Menu
    "Connect Wifi",
    "WiFi AP",
    "Disconnect",
    "Wifi Atks",
    "Evil Portal",
    "Listen TCP",
    "Client TCP",
    "TelNET",
    "SSH",
    "DPWO",
    "Raw Sniffer",
    "Scan Hosts",
    "Wireguard",
    "Brucegotchi",
    "Add Evil Wifi",
    "Remove Evil Wifi",
    "AP info",
    
    // Config Menu
    "Brightness",
    "Dim Time",
    "Orientation",
    "UI Color",
    "UI Theme",
    "LED Color",
    "LED Brightness",
    "Led Blink On/Off",
    "Sound On/Off",
    "Startup WiFi",
    "Startup App",
    "Network Creds",
    "Sleep",
    "Factory Reset",
    "Restart",
    "Turn-off",
    "Deep Sleep",
    "About",
    "Dev Mode",
    "I2C Finder",
    "CC1101 Pins",
    "NRF24  Pins",
    "SDCard Pins",
    "Language",
    
    // Common Messages
    "Back",
    "Cancel",
    "Select",
    "Save",
    "Delete",
    "Error",
    "Success",
    "Loading",
    "Disabled",
    "Enable",
    "Disable",
    
    // Settings strings
    "Default",
    "White",
    "Red",
    "Green",
    "Blue",
    "Light Blue",
    "Yellow",
    "Magenta",
    "Orange",
    "Custom Ui Color",
    "Invert Color",
    "Sound off",
    "Sound on",
    "Led Blink off",
    "Led Blink on",
};

// Portuguese BR strings
static const char* const strings_pt_br[] = {
    // Main Menu Items
    "WiFi",
    "BLE",
    "RF",
    "IR",
    "RFID",
    "NRF24",
    "FM",
    "Arquivos",
    "GPS",
    "Interpretador JS",
    "Outros",
    "Relógio",
    "Conectar",
    "Config",
    
    // WiFi Menu
    "Conectar WiFi",
    "WiFi AP",
    "Desconectar",
    "Ataques WiFi",
    "Portal Malicioso",
    "Escutar TCP",
    "Cliente TCP",
    "TelNET",
    "SSH",
    "DPWO",
    "Sniffer Bruto",
    "Escanear Hosts",
    "Wireguard",
    "Brucegotchi",
    "Adicionar WiFi Mal.",
    "Remover WiFi Mal.",
    "Info do AP",
    
    // Config Menu
    "Brilho",
    "Tempo Desligar",
    "Rotação",
    "Cor da Interface",
    "Tema da Interface",
    "Cor do LED",
    "Brilho do LED",
    "Piscar LED Lig/Des",
    "Som Ligado/Desl.",
    "WiFi ao Iniciar",
    "App ao Iniciar",
    "Credenciais Rede",
    "Suspender",
    "Reset de Fábrica",
    "Reiniciar",
    "Desligar",
    "Suspensão Profunda",
    "Sobre",
    "Modo Desenvolvedor",
    "Localizador I2C",
    "Pinos CC1101",
    "Pinos NRF24",
    "Pinos SDCard",
    "Idioma",
    
    // Common Messages
    "Voltar",
    "Cancelar",
    "Selecionar",
    "Salvar",
    "Excluir",
    "Erro",
    "Sucesso",
    "Carregando",
    "Desabilitado",
    "Habilitar",
    "Desabilitar",
    
    // Settings strings
    "Padrão",
    "Branco",
    "Vermelho",
    "Verde",
    "Azul",
    "Azul Claro",
    "Amarelo",
    "Magenta",
    "Laranja",
    "Cor Personalizada",
    "Inverter Cor",
    "Som desligado",
    "Som ligado",
    "Piscar LED deslig.",
    "Piscar LED ligado",
};

const char* getString(int stringId) {
    // Get language from config
    int lang = bruceConfig.language;
    
    // Validate stringId
    int maxStrings = sizeof(strings_en) / sizeof(strings_en[0]);
    if (stringId < 0 || stringId >= maxStrings) {
        return "???";
    }
    
    // Return appropriate string based on language
    if (lang == LANG_PT_BR) {
        return strings_pt_br[stringId];
    }
    return strings_en[stringId];
}
