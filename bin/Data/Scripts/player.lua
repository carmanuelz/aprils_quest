Rotator = ScriptObject()

function Rotator:Start()
    print("iniciando el scrip")
end

function Rotator:SetRotationSpeed(var)    
    print(var)
end

function Rotator:Update(timeStep)
    print("este es el bucle de actualizacion") 
end