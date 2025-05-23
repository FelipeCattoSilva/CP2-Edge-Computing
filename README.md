Sistema de Monitoramento Ambiental com Arduino
Este projeto é um sistema de monitoramento ambiental desenvolvido para Arduino. Ele utiliza sensores para medir temperatura, umidade e luminosidade, exibindo os dados em um display LCD. Além disso, o sistema inclui alertas visuais e sonoros para condições fora dos limites definidos e registra os dados em intervalos regulares.
Componentes Utilizados

Sensor DHT22: Mede temperatura e umidade
LDR: Sensor de luminosidade (Resistor Dependente de Luz)
Display LCD 16x2 com interface I2C: Exibe os dados dos sensores
Módulo RTC DS1307: Fornece data e hora em tempo real
Buzzer: Emite alertas sonoros
LEDs: Vermelho, amarelo e verde para alertas visuais
EEPROM: Armazena os dados registrados

Conexões
Conecte os componentes ao Arduino conforme abaixo:

DHT22: Pino 2
LDR: Pino A0
Buzzer: Pino 3
LEDs: 
Vermelho: Pino 4
Amarelo: Pino 5
Verde: Pino 6


LCD: Conectado via I2C (endereço 0x27)
RTC: Conectado via I2C

Bibliotecas Necessárias
Para compilar o código, instale as seguintes bibliotecas no Arduino IDE:

LiquidCrystal_I2C
DHT
Wire
RTClib
EEPROM

Você pode instalá-las através do Gerenciador de Bibliotecas no Arduino IDE.
Funcionamento do Código
O código realiza as seguintes funções:

Inicialização: Configura os pinos e inicia as bibliotecas. Exibe uma animação de carregamento com um cavalo se movendo no LCD.
Leitura dos Sensores: A cada 2 segundos, lê temperatura e umidade do DHT22 e luminosidade do LDR.
Média de Luminosidade: Calcula a média das últimas 10 leituras de luminosidade para suavizar variações.
Exibição: Alterna no LCD entre:
Primeira linha: Temperatura e umidade
Segunda linha: Luminosidade e data/hora


Alertas: Verifica se os valores estão dentro dos limites:
Fora dos limites: Aciona LEDs (vermelho ou amarelo) e buzzer
Dentro dos limites: Acende o LED verde


Registro: A cada minuto, armazena os dados (data-hora, temperatura, umidade e luminosidade) na EEPROM.

Limites para Alertas

Temperatura: 6°C a 20°C
Umidade: 60% a 80%
Luminosidade: 0.1% a 350%Nota: A luminosidade é mapeada de 0 a 100%, então os limites de 0.1 e 350 podem precisar de ajuste. Sugere-se usar valores entre 0 e 100.

Animação de Carregamento
Durante a inicialização, o sistema exibe uma animação de um cavalo se movendo no LCD, acompanhada por uma barra de progresso de 0 a 100%. Após a animação, mostra "UnlimitTech Solution" e "Bem-Vindo!".
Uso
Siga os passos abaixo para configurar e usar o projeto:

Conecte os Componentes: Ligue todos os componentes ao Arduino conforme a seção "Conexões".
Instale as Bibliotecas: Adicione as bibliotecas necessárias ao Arduino IDE.
Carregue o Código: Faça upload do código para o Arduino usando o Arduino IDE.
Inicialização: O sistema começará com a animação de carregamento e, em seguida, exibirá os dados dos sensores.
Monitoramento: Os valores serão atualizados a cada 2 segundos, com alertas acionados se estiverem fora dos limites.
Registro: Os dados são salvos na EEPROM a cada minuto.

Nota: O código armazena os dados na EEPROM, mas não inclui uma forma de recuperá-los diretamente. Para acessar os logs, você pode adicionar funcionalidades ao código, como exibi-los via monitor serial.

Esperamos que este projeto seja útil! Se precisar de ajustes ou tiver dúvidas, sinta-se à vontade para abrir uma issue ou contribuir com o repositório.
