using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Roboarm : MonoBehaviour
{
    [Header("Velocidades de rotação dos servos")]
    public float servo1RotationSpeed = 100;
    public float servo2RotationSpeed = 100;
    public float servo3RotationSpeed = 100;
    public float servo4RotationSpeed = 100;

    private const float TEMPO_TIMER = 0.5f;

    private const string ROBOARM_SERVO_3_PATH = "servo_3";
    private const string ROBOARM_SERVO_1_PATH = ROBOARM_SERVO_3_PATH + "/servo_1";
    private const string ROBOARM_01_PATH = ROBOARM_SERVO_1_PATH + "/roboarm_alavanca/roboarm_01";
    private const string ROBOARM_05_PATH = ROBOARM_SERVO_1_PATH + "/roboarm_alavanca/roboarm_05";
    private const string ROBOARM_06_PATH = ROBOARM_SERVO_1_PATH + "/roboarm_alavanca/roboarm_11/roboarm_06";
    private const string ROBOARM_07_PATH = ROBOARM_SERVO_3_PATH + "/roboarm_07_master";
    private const string ROBOARM_SERVO_4_PATH = ROBOARM_SERVO_3_PATH + "/servo_4";
    private const string ROBOARM_09_PATH = ROBOARM_SERVO_4_PATH + "/roboarm_10/roboarm_09_master";
    private const string ROBOARM_11_PATH = ROBOARM_SERVO_1_PATH + "/roboarm_alavanca/roboarm_11";
    private const string ROBOARM_SUPORTE_GARRA = ROBOARM_SERVO_1_PATH + "/roboarm_alavanca/roboarm_01/suporte_garra";
    private const string ROBOARM_GARRA_DIREITA = ROBOARM_SUPORTE_GARRA + "/garra_direita";
    private const string ROBOARM_GARRA_ESQUERDA = ROBOARM_SUPORTE_GARRA + "/garra_esquerda";
    private const string ROBOARM_SERVO_2_PATH = ROBOARM_SUPORTE_GARRA + "/servo_2";
    private const string ROBOARM_GARRA_ALAVANCA_MECANISMO = ROBOARM_SERVO_2_PATH + "/garra_mecanismo_2";
    private Transform _roboarmServo1, _roboarmServo2, _roboarmServo3, _roboarmServo4;
    private Transform _roboarm01, _roboarm05, _roboarm06, _roboarm07,_roboarm09,_roboarm11;
    private Transform _roboarmSuporteGarra, _roboarmGarraDireita, _roboarmGarraEsquerda, _roboarmGarraAlavancaMecanismo;
    private Vector3 _suporteGarraPosicaoFrente;

    public static Roboarm instancia;

    private void Awake() {
        instancia = this;
    }
    
    void Start()
    {
        _roboarmServo1 = transform.Find(ROBOARM_SERVO_1_PATH);
        _roboarmServo2 = transform.Find(ROBOARM_SERVO_2_PATH);
        _roboarmServo3 = transform.Find(ROBOARM_SERVO_3_PATH);
        _roboarmServo4 = transform.Find(ROBOARM_SERVO_4_PATH);
        _roboarm01 = transform.Find(ROBOARM_01_PATH);
        _roboarm05 = transform.Find(ROBOARM_05_PATH);
        _roboarm06 = transform.Find(ROBOARM_06_PATH);
        _roboarm07 = transform.Find(ROBOARM_07_PATH);
        _roboarm09 = transform.Find(ROBOARM_09_PATH);
        _roboarmServo4 = transform.Find(ROBOARM_SERVO_4_PATH);
        _roboarm11 = transform.Find(ROBOARM_11_PATH);
        _roboarmSuporteGarra = transform.Find(ROBOARM_SUPORTE_GARRA);
        _roboarmGarraDireita = transform.Find(ROBOARM_GARRA_DIREITA);
        _roboarmGarraEsquerda = transform.Find(ROBOARM_GARRA_ESQUERDA);
        _roboarmGarraAlavancaMecanismo = transform.Find(ROBOARM_GARRA_ALAVANCA_MECANISMO);

        _suporteGarraPosicaoFrente = _roboarmSuporteGarra.rotation.eulerAngles;

        StartCoroutine(TimerRequisicoes());
    }

    // Update is called once per frame
    void Update()
    {
        MechanicRelation();
        Movement();
    }

    public float GetAngleRotationServo1()
    {
        if(_roboarmServo1.localEulerAngles.x > 180)
        {
            float amount = _roboarmServo1.localEulerAngles.x > 350 ? 360 - _roboarmServo1.localEulerAngles.x : 10;
            return 40 - amount;
        }
        else
            return _roboarmServo1.localEulerAngles.x + 40;
    }
    
    public void SetAngleRotationServo1(float angle)
    {
        if(angle >= 30 && angle <= 130)
            _roboarmServo1.localEulerAngles = new Vector3(angle - 40, 0, 0);
    }

    public float GetAngleRotationServo2()
    {
        float amount = _roboarmServo2.localEulerAngles.z >= 280 ? 360 - _roboarmServo2.localEulerAngles.z : 0;
        return 155 - amount;
    }
    
    public void SetAngleRotationServo2(float angle)
    {
        if(angle >= 75 && angle <= 155)
            Debug.Log("angle => "+angle);
            _roboarmServo2.localEulerAngles = new Vector3(0, 0, 0 - (155 - angle));
    }

    public float GetAngleRotationServo3()
    {
        if(_roboarmServo3.localRotation.eulerAngles.y > 180)
        {
            float amount = _roboarmServo3.localEulerAngles.y >= 270 ? 360 - _roboarmServo3.localEulerAngles.y : 270;
            return amount + 90;
        }
        else
            return 90 - _roboarmServo3.localEulerAngles.y;
    }
    
    public void SetAngleRotationServo3(float angle)
    {
        if(angle >= 0 && angle <= 180)
            _roboarmServo3.localEulerAngles = new Vector3(0, -angle + 90, 0);
    }

    public float GetAngleRotationServo4()
    {
        if(_roboarmServo4.localRotation.eulerAngles.x > 180)
        {
            float amount = _roboarmServo4.localEulerAngles.x >= 275 ? 360 - _roboarmServo4.localEulerAngles.x : 275;
            return amount + 66;
        }
        else
            return 66 - _roboarmServo4.localEulerAngles.x;
    }

    public void SetAngleRotationServo4(float angle)
    {
        if(angle >= 10 && angle <= 151)
            _roboarmServo4.localEulerAngles = new Vector3(66 - angle, 0, 0);
    }

    private Vector3 InvertRotationAxisX(Transform transformReference)
    {
        Vector3 vector = transformReference.localRotation.eulerAngles;
        vector.x *= -1;
        return vector;
    }
    
    private Vector3 InvertRotationAxisZ(Transform transformReference)
    {
        Vector3 vector = transformReference.localRotation.eulerAngles;
        vector.z *= -1;
        return vector;
    }

    private void MechanicRelation()
    {
        _roboarm01.localEulerAngles = InvertRotationAxisX(_roboarmServo1) - InvertRotationAxisX(_roboarmServo4);
        _roboarm05.eulerAngles = _roboarm01.eulerAngles;
        _roboarm06.eulerAngles = _roboarm05.eulerAngles;
        _roboarm07.localEulerAngles = _roboarmServo1.localEulerAngles;
        _roboarm11.localEulerAngles = InvertRotationAxisX(_roboarmServo1);
        _roboarm09.localEulerAngles = _roboarmServo1.localEulerAngles + InvertRotationAxisX(_roboarmServo4);
        _roboarmGarraEsquerda.localEulerAngles = _roboarmServo2.localEulerAngles;
        _roboarmGarraDireita.localEulerAngles = InvertRotationAxisZ(_roboarmServo2);
        _roboarmGarraAlavancaMecanismo.localEulerAngles = InvertRotationAxisZ(_roboarmServo2);
        _roboarmSuporteGarra.eulerAngles = new Vector3(_suporteGarraPosicaoFrente.x, _roboarmSuporteGarra.eulerAngles.y, _roboarmSuporteGarra.eulerAngles.z);
    }

    private void Movement()
    {
        float servo1NewRotation = Input.GetAxisRaw("Vertical") * servo1RotationSpeed * Time.deltaTime;
        float servo2NewRotation = Input.GetAxisRaw("HorizontalR") * servo2RotationSpeed * Time.deltaTime;
        float servo3NewRotation = Input.GetAxisRaw("Horizontal") * servo3RotationSpeed * Time.deltaTime;
        float servo4NewRotation = Input.GetAxisRaw("VerticalR") * servo4RotationSpeed * Time.deltaTime;
        
        _roboarmServo1.Rotate(servo1NewRotation, 0, 0);
        _roboarmServo2.Rotate(0, 0, servo2NewRotation);
        _roboarmServo3.Rotate(0, servo3NewRotation, 0);
        _roboarmServo4.Rotate(servo4NewRotation, 0, 0);

        TravaServo1();
        TravaServo2();
        TravaServo3();
        TravaServo4();

        PublicaServo(servo1NewRotation, servo2NewRotation, servo3NewRotation, servo4NewRotation);
    }

    private void PublicaServo(float servo1, float servo2, float servo3, float servo4)
    {
        float AngleRotationServo1 = GetAngleRotationServo1();
        float AngleRotationServo2 = GetAngleRotationServo2();
        float AngleRotationServo3 = GetAngleRotationServo3();
        float AngleRotationServo4 = GetAngleRotationServo4();

        // Debug.Log("AngleRotationServo1 "+AngleRotationServo1);
        // Debug.Log("AngleRotationServo2 "+AngleRotationServo2);
        // Debug.Log("AngleRotationServo3 "+AngleRotationServo3);
        // Debug.Log("AngleRotationServo4 "+AngleRotationServo4);

        Debug.Log("servo1 "+servo1);
        Debug.Log("servo2 "+servo2);
        Debug.Log("servo3 "+servo3);
        Debug.Log("servo4 "+servo4);

        if((float)0 != servo1)
        {
            MqttManager.instancia.MoveRoboArmServo1(AngleRotationServo1);
            Debug.Log("servo1 "+servo1);
        }
        else if((float)0 != servo2)
        {
            MqttManager.instancia.MoveRoboArmServo2(AngleRotationServo2);
            Debug.Log("servo2 "+servo2);
        }
        else if((float)0 != servo3)
        {
            MqttManager.instancia.MoveRoboArmServo3(AngleRotationServo3);
            Debug.Log("servo3 "+servo3);
        }
        else if((float)0 != servo4)
        {
            MqttManager.instancia.MoveRoboArmServo4(AngleRotationServo4);
            Debug.Log("servo4 "+servo4);
        }

    }

    private void TravaServo1()
    {
        if(_roboarmServo1.localEulerAngles.x < 350)  // max 130
        {
            if(_roboarmServo1.localEulerAngles.x > 180 && _roboarmServo1.localEulerAngles.x < 350)
                _roboarmServo1.localEulerAngles = new Vector3(350, 0, 0);
            else if (_roboarmServo1.localEulerAngles.x < 90 && _roboarmServo1.localEulerAngles.y != 0)
                _roboarmServo1.localEulerAngles = new Vector3(90, 0, 0);
        }
    }

    private void TravaServo2()
    {
        if(_roboarmServo2.localEulerAngles.z < 280 && _roboarmServo2.localEulerAngles.z > 0)
        {
            _roboarmServo2.localEulerAngles = _roboarmServo2.localEulerAngles.z > 180 ? new Vector3(0, 0, 280) : new Vector3(0, 0, 0);
        }
    }

    private void TravaServo3()
    {
        if(_roboarmServo3.localEulerAngles.y > 90)
        {
            if(_roboarmServo3.localEulerAngles.y > 180 && _roboarmServo3.localEulerAngles.y < 270)
                _roboarmServo3.localEulerAngles = new Vector3(0, 270, 0);
            else if (_roboarmServo3.localEulerAngles.y < 180)
                _roboarmServo3.localEulerAngles = new Vector3(0, 90, 0);
        }
    }

    private void TravaServo4()
    {
        if(_roboarmServo4.localEulerAngles.x > 56)
        {
            if(_roboarmServo4.localEulerAngles.x > 180 && _roboarmServo4.localEulerAngles.x < 275)
                _roboarmServo4.localEulerAngles = new Vector3(275, 0, 0);
            else if (_roboarmServo4.localEulerAngles.x < 180)
                _roboarmServo4.localEulerAngles = new Vector3(56, 0, 0);
        }
    }

    private IEnumerator TimerRequisicoes()
    {
        yield return new WaitForSeconds(TEMPO_TIMER);
        // Seu código aqui
        

        yield return TimerRequisicoes();
    }
    
}
