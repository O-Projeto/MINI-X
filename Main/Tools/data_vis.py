from bottle import route, run



from bottle import route, request
# from config import read_config, write_config

color =  "#eb3434"

@route('/')
def index():
    
    return f"""
    <!DOCTYPE html>
    <html>
    <head>
    <style>
    .grid-container {{
    display: grid;
    grid-template-columns: auto auto auto;
    background-color: {color};
    padding: 10px;
    }}
    .grid-item {{
    background-color: rgba(255, 255, 255, 0.8);
    border: 1px solid rgba(0, 0, 0, 0.8);
    padding: 20px;
    font-size: 30px;
    text-align: center;
    }}
    </style>
    </head>
    <body>
    <div class="grid-container">
    <div class="grid-item">1</div>
    <div class="grid-item">2</div>
    <div class="grid-item">3</div>  
    <div class="grid-item">4</div>
    <div class="grid-item">5</div>
    <div class="grid-item">6</div>  
    <div class="grid-item">7</div>
    <div class="grid-item">8</div>
    <div class="grid-item">9</div>  
    </div>

    </body>"""




run(host='localhost', port=8080, debug=True)