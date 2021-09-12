class ds3218mg{
  public:
    ds3218mg();
    ds3218mg(unsigned int pin);
    int attach(unsigned int pin);
    int write(int val);
   private:
    Servo m_Servo;
};
