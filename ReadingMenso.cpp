//#pragma hdrstop
#include "ReadingMenso.h"
#include "New.h"

//------------------------------------------------------------
/*
void
ReadingSimpleMenso::
Recursive(String Tkind, Unsigned offset)
{
    AnalyzingKind AK(Tkind);
    if (AK.IsFunctional())
    {
        String may_be_lparen = ML.deco[offset + 1];
        if (may_be_lparen == "(")
        {
            std_menso += "(";
            GetArgList_And_SetKind(Tkind, offset + 2);
            Recursive(AK.ReturnType(), end);
        }
        else
        {
            kind = Tkind;
            end = offset;
        }
    }
    else
    {
        kind = Tkind;
        end = offset;
    }
}

void
ReadingSimpleMenso::
Run(Unsigned offset)
{
    String thing_name = ML.deco[offset];
    std_menso += thing_name;

    String name_and_type = IsThing(ML, thing_name).name_and_type; // we have thing name but we need complete thing (e.g. name +tupe)
    Thing T(name_and_type);
    Recursive(T.kind, offset);

    
}

ReadingSimpleMenso::
ReadingSimpleMenso(MensoList& ML, Unsigned offset)
    : ML(ML)
{
        Run(offset);
}*/
//_______________________________________
/*
void ReadingUnit::
GetArgList_And_SetKind(String kind, Unsigned offset)
{
    AnalyzingKind AK(kind);
    Unsigned n = AK.ParameterNumber();

    //Unsigned mutable_offset = offset;

    for (Unsigned i = 1; i <= n; i++)
    {
        ReadingMenso RM(ML, offset);
        std_menso += RM.std_menso;
        if (AK.nthParameter(i) == RM.kind)// if ith arg has expected kind
        {
            String to_be_comma_or_lp = ML.deco[RM.end + 1];
            end = RM.end + 1;
            if (i < n)
            {
                if (to_be_comma_or_lp == ",")
                    std_menso += ",";
                else
                    ReportError("comma expected", RM.end + 1);
            }
            else
            {
                if (to_be_comma_or_lp == ")")
                    std_menso += ")";
                else
                    ReportError(") expected", RM.end + 1);
            }
        }
        else
            ReportError("bad argument", RM.end);
        offset = RM.end + 2;
    }
    this->kind = AK.ReturnType(); // set kind
}


void
ReadingUnit::
Run(Unsigned offset)
{
    String gate = ML.deco[offset];

    IsThing* pit;
    IsOperator* pio;

    if ((pit = new IsThing(ML, gate)) -> is_thing)   // if it's a thing
    {
        kind = pit->kind;
        std_menso += gate;
        end = offset;
        delete pit;

        // ReadingSimpleMenso RSM(ML, offset);
        // kind = RSM.kind;
        // end = RSM.end;
        // std_menso += RSM.std_menso;
    }
    else if ((pio = new IsOperator(ML, gate))->is_operator)
    {
        String thing = pio->operator_thing; // we have thing name *& want complete thing
        ThingOrOperator T(thing);
        AnalyzingKind AK(T.kind);
        if (AK.ParameterNumber() == 1)
        {
            ReadingUnit RU(ML, offset + 1);
            std_menso += String("(") + T.name + RU.std_menso + ")";
            kind = AK.ReturnType();
            end = RU.end;
        }
        else
            ReportError("this operator needs two arguments", offset);
        delete pio;
    }
    else if (gate == "(")
    {
        String next = ML.deco[offset + 1];
        if (next == "all" || next == "some")
        {
            ReadingQuan RQ(ML, offset);
            kind = RQ.kind;
            std_menso += RQ.std_menso;
            end = RQ.end;
        }
        else if (next == "if")
        {
            ReadingIf RI(ML, offset);
            kind = RI.kind;
            std_menso += RI.std_menso;
            end = RI.end;
        }
        else
        {
            ReadingMenso RM(ML, offset + 1);
            kind = RM.kind;
            String to_be_RP = ML.deco[RM.end + 1];
            if (to_be_RP != ")")
                ReportError("')' expected", RM.end + 1);
            std_menso += RM.std_menso;
            end = RM.end + 1;
        }
    }
    else
        ReportError("object not defined or '(' missed", offset);

    while (true)
    {
        AnalyzingKind AK(kind);
        if (AK.IsFunctional())
        {
            String next = ML.deco[end + 1];
            if (next != "(")
                return;
            std_menso += "(";

            //ReadingSimpleMenso RSM(ML); // just using ReadingSimpleMenso::GetArgList_And_SetKind method
            //RSM.GetArgList_And_SetKind(kind, end + 2);

            GetArgList_And_SetKind(kind, end + 2);
            // std_menso += RSM.std_menso;
            //end = RSM.end;
            //kind = RSM.kind;
        }
        else
            return;
    }
}*/

/*
void
ReadingQuan::
Run(Unsigned offset)
{
    String to_be_RP = ML.text_item[offset];
    if (to_be_RP != "(")
        ReportError("( expected", offset);
    std_menso += "(";

    String next = ML.text_item[offset + 1];
    if (next == "all")
        std_menso += "all ";
    else if (next == "some")
        std_menso += "some ";
    else
        ReportError("'all' or 'some' expected", offset + 1);

    New N(ML);
    ThingOrOperator T = N.NewThing(offset + 2); // This also adds T.thing to ML.things
    end = N.end;

    //offset = offset + 2;
  //  ReadingKind RK(ML, offset + 2);
   // String name = ML.deco[offset + 3];
    //Thing T(name, RK.kind);

   // if (ML.IsMultipleName(T.name) == true)
     //   ReportError("multiple name", offset + 3);
    //ML.things << T.thing;

    next = ML.text_item[end + 1];
    if(next != ")")
        ReportError("')' expected", end + 1);

    std_menso += T.thing + ")(";



    //if (RK.parens_found_for_main_type == true)  // if it has parameters
     //   end = RK.end + 1;
    //else
      //  end = RK.end + 2; // because in "prop p;" the end is "prop" 

    String start_of_menso_part = ML.text_item[end + 2];
    if (start_of_menso_part == "(")
    {
        ReadingUnit RU(ML, end + 2); // if Reading Menso operators will be included
        if (RU.kind != "prop")
            ReportError("quantifier menso must have prop kind", RU.end);
        end = RU.end;
        std_menso += RU.std_menso + ")";
    }
    else
    {
        ReadingMenso RM(ML, end + 2);
        if (RM.kind != "prop")
            ReportError("quantifier menso must have prop kind", RM.end);
        end = RM.end;
        std_menso += RM.std_menso + ")";
    }

    kind = "prop";

    Replace R(std_menso);
    std_menso = R.Substitute(T.name, R.NewTempName());
    ML.things.FindAndDelIfAny(T.thing);
}*/

/*
void
ReadingIf::
Run(Unsigned offset)
{
    Unsigned mutable_offset = offset;
    String to_be_RP = ML.text_item[mutable_offset];
    if (to_be_RP != "(")
        ReportError("( expected", mutable_offset);
    std_menso += "(";
    String next = ML.text_item[mutable_offset + 1];

    if (next == "if")
        std_menso += "if ";
    else
        ReportError("'if' expected", mutable_offset + 1);

    mutable_offset = mutable_offset + 2;
    ReadingMenso RM(ML, mutable_offset);
    mutable_offset = RM.end;
    if (RM.kind != "prop")
        ReportError("menso must be prop", mutable_offset);

    std_menso += RM.std_menso;

    next = ML.text_item[mutable_offset + 1];
    if (next != ")")
        ReportError("')' expected", mutable_offset + 1);
    end = mutable_offset + 1;

    std_menso += ")(";

    String start_of_menso_part = ML.text_item[end + 1];
    if (start_of_menso_part == "(")
    {
        ReadingUnit RU(ML, end + 1); // if Reading Menso operators will be included
        if (RU.kind != "prop")
            ReportError("quantifier menso must have prop kind", RU.end);
        end = RU.end;
        std_menso += RU.std_menso + ")";
    }
    else
    {
        ReadingMenso RT(ML, end + 1);
        if (RT.kind != "prop")
            ReportError("quantifier menso must have prop kind", RT.end);
        end = RT.end;
        std_menso += RT.std_menso + ")";
    }
    kind = "prop";
}
*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ReadingMenso::MensoKindEnd_ReturnType
ReadingMenso::
GetArgList_And_Kind(const String& _kind, Unsigned offset) const
{
    MensoKindEnd_ReturnType ret;
    AnalyzingKind AK(_kind);
    Unsigned n = AK.ParameterNumber();

    //Unsigned mutable_offset = offset;

    for (Unsigned i = 1; i <= n; i++)
    {
        ReadingMenso RM(ML, offset);
        ret.menso += RM.std_menso;
        if (AK.nthParameterKind(i) == RM.kind)// if ith arg has expected kind
        {
            String to_be_comma_or_lp = ML.text_item[RM.end + 1];
            ret.end = RM.end + 1;
            if (i < n)
            {
                if (to_be_comma_or_lp == ",")
                    ret.menso += ",";
                else
                    ReportError("comma expected", RM.end + 1);
            }
            else
            {
                if (to_be_comma_or_lp == ")")
                    ret.menso += ")";
                else
                    ReportError(") expected", RM.end + 1);
            }
        }
        else
            ReportError("bad argument", RM.end);
        offset = RM.end + 2;
    }
    ret.kind = AK.ReturnType(); // set new kind
    return ret;
}
ReadingMenso::MensoKindEnd_ReturnType
ReadingMenso::
ReadIf(Unsigned offset) const
{
    MensoKindEnd_ReturnType ret;

    //Unsigned mutable_offset = offset;
    String to_be_RP = ML.text_item[offset];
    if (to_be_RP != "(")
        ReportError("( expected", offset);
    ret.menso += "(";
    String next = ML.text_item[offset + 1];

    if (next == "if")
        ret.menso += "if ";
    else
        ReportError("'if' expected", offset + 1);

    ReadingMenso RM(ML, offset + 2);
    //offset = RM.end;
    if (RM.kind != "prop")
        ReportError("menso must be prop", RM.end);

    ret.menso += RM.std_menso;

    next = ML.text_item[RM.end + 1];
    if (next != ")")
        ReportError("')' expected", RM.end + 1);
    // ret.end = RM.end + 1;

    ret.menso += ")(";

    String start_of_menso_part = ML.text_item[RM.end + 2];
    if (start_of_menso_part == "(")
    {
        // ReadingUnit RU(ML, end + 1); // if Reading Menso operators will be included
        auto MKE = ReadUnit(RM.end + 2);
        if (MKE.kind != "prop")
            ReportError("quantifier menso must have prop kind", MKE.end);
        ret.end = MKE.end;
        ret.menso += MKE.menso + ")";
    }
    else
    {
        ReadingMenso _RM(ML, RM.end + 2);
        if (_RM.kind != "prop")
            ReportError("quantifier menso must have prop kind", _RM.end);
        ret.end = _RM.end;
        ret.menso += _RM.std_menso + ")";
    }
    ret.kind = "prop";
    return ret;
}

ReadingMenso::MensoKindEnd_ReturnType
ReadingMenso::
ReadQuan(Unsigned offset) const
{
    MensoKindEnd_ReturnType ret;

    String to_be_RP = ML.text_item[offset];
    if (to_be_RP != "(")
        ReportError("( expected", offset);
    ret.menso += "(";

    String next = ML.text_item[offset + 1];
    if (next == "all")
        ret.menso += "all ";
    else if (next == "some")
        ret.menso += "some ";
    else
        ReportError("'all' or 'some' expected", offset + 1);

    New N(ML);
    ThingOrOperator T = N.NewThing(offset + 2); // This also adds T.thing to ML.things
    ret.end = N.end;

    /*  //offset = offset + 2;
      ReadingKind RK(ML, offset + 2);
      String name = ML.deco[offset + 3];
      Thing T(name, RK.kind);

      if (ML.IsMultipleName(T.name) == true)
          ReportError("multiple name", offset + 3);
      ML.things << T.thing;*/

    next = ML.text_item[N.end + 1];
    if (next != ")")
        ReportError("')' expected", N.end + 1);

    ret.menso += T.thing + ")(";


    /*if (RK.parens_found_for_main_type == true)  // if it has parameters
        end = RK.end + 1;
    else
        end = RK.end + 2; // because in "prop p;" the end is "prop" */

    String start_of_menso_part = ML.text_item[N.end + 2];
    if (start_of_menso_part == "(")
    {
        // ReadingUnit RU(ML, end + 2); // if Reading Menso operators will be included
        auto MKE = ReadUnit(N.end + 2);
        if (MKE.kind != "prop")
            ReportError("quantifier menso must have prop kind", MKE.end);
        ret.end = MKE.end;
        ret.menso += MKE.menso + ")";
    }
    else
    {
        ReadingMenso RM(ML, N.end + 2);
        if (RM.kind != "prop")
            ReportError("quantifier menso must have prop kind", RM.end);
        ret.end = RM.end;
        ret.menso += RM.std_menso + ")";
    }

    ret.kind = "prop";

    Replace R(ret.menso);
    ret.menso = R.Substitute(T.name, R.NewTempName());
    ML.things.FindAndDelIfAny(T.thing);
    return ret;
}

ReadingMenso::MensoKindEnd_ReturnType
ReadingMenso::
ReadUnit(Unsigned offset) const // a unit is a smallest menso that can be read from an offset, e.g. x + y is not a unit, but (x + y) is a unit in fact a unit is a menso with no outer binary operators
{
    MensoKindEnd_ReturnType ret; // to be returned

    String gate = ML.text_item[offset];

    IsThing* pit;
    IsOperator* pio;

    if ((pit = new IsThing(ML, gate))->is_thing)   // if it's a thing
    {
        ret.kind = pit->kind;
        ret.menso += gate;
        ret.end = offset;
        delete pit;

        /*ReadingSimpleMenso RSM(ML, offset);
        kind = RSM.kind;
        end = RSM.end;
        std_menso += RSM.std_menso;*/
    }
    else if ((pio = new IsOperator(ML, gate))->is_operator)
    {
        String thing = pio->operator_thing; // we have operator name *& want complete thing
        ThingOrOperator T(thing);
        AnalyzingKind AK(T.kind);
        if (AK.ParameterNumber() == 1)
        {
            //ReadingUnit RU(ML, offset + 1);

            auto MKE = ReadUnit(offset + 1);
            if (T.name == "!")
            {
                AnalyzingMenso AM(ML, MKE.menso);
                if (AM.unary_operator_type && AM.op == '!')
                    ret.menso += AM.part;
                else
                    ret.menso += MakeUnaryOp("!", MKE.menso); // CM.Negate(MKE.menso);
            }
            else
                ret.menso = ret.menso + "(" + T.name + MKE.menso + ")";
            ret.kind = AK.ReturnType();
            ret.end = MKE.end;
        }
        else
            ReportError("this operator needs two arguments", offset);
        delete pio;
    }
    else if (gate == "(")
    {
        String next = ML.text_item[offset + 1];
        if (next == "all" || next == "some")
        {
            // ReadingQuan RQ(ML, offset);
            auto MKE = ReadQuan(offset);
            ret.kind = MKE.kind;
            ret.menso += MKE.menso;
            ret.end = MKE.end;
        }
        else if (next == "if")
        {
            // ReadingIf RI(ML, offset);
            auto MKE = ReadIf(offset);
            ret.kind = MKE.kind;
            ret.menso += MKE.menso;
            ret.end = MKE.end;
        }
        else
        {
            ReadingMenso RM(ML, offset + 1);
            ret.kind = RM.kind;
            String to_be_RP = ML.text_item[RM.end + 1];
            if (to_be_RP != ")")
                ReportError("')' expected", RM.end + 1);
            ret.menso += RM.std_menso;
            ret.end = RM.end + 1;
        }
    }
    else
        ReportError("object not defined or '(' missed", offset);

    while (true)
    {
        AnalyzingKind AK(ret.kind);
        if (AK.IsFunctional())
        {
            String next = ML.text_item[ret.end + 1];
            if (next != "(")
                return ret;
            ret.menso += "(";

            //ReadingSimpleMenso RSM(ML); // just using ReadingSimpleMenso::GetArgList_And_SetKind method
            //RSM.GetArgList_And_SetKind(kind, end + 2);

            auto MKE = GetArgList_And_Kind(ret.kind, ret.end + 2);
            ret.menso += MKE.menso;
            ret.kind = MKE.kind;
            ret.end = MKE.end;
            // std_menso += RSM.std_menso;
            //end = RSM.end;
            //kind = RSM.kind;
        }
        else
            return ret;
    }
} // ReadUnit

void
ReadingMenso::
ConstructMensoWithOperators(List& Units, List& Kinds, List& Operators)
{
    Unsigned op_len = Operators.Size();
    if (op_len > 0)
    {
        vector<Unsigned> v;
        for (Unsigned i = 0; i < op_len; i++)
        {
            IsOperator io(ML, Operators[i]);
            v.push_back(io.priority);
        }
        Unsigned max_i = 0; // v[max_i] will be the max priority
        for (Unsigned i = 0; i < op_len; i++)
        {
            if (v[i] > v[max_i])
                max_i = i;
        }
        //-------------------------------------
        IsOperator io(ML, Operators[max_i]);
        ThingOrOperator T(io.operator_thing); // decompose to name & kind

        if (T.name == "=")
        {
            if (String(Kinds[max_i]) == String(Kinds[max_i + 1]))
            {
                Units[max_i] = MakeBinaryOp(Units[max_i], Operators[max_i], Units[max_i + 1]);// String('(') + Units[max_i] + Operators[max_i] + Units[max_i + 1] + ")";
                Kinds[max_i] = "prop";
                Units[max_i + 1] = 0; // delete it
                Kinds[max_i + 1] = 0; // delete it
                Operators[max_i] = 0; // delete it
                ConstructMensoWithOperators(Units, Kinds, Operators); // recursive
            }
            else
                ReportError("mismatch kind for operartor", end);
        }
        else
        {
            AnalyzingKind AK(T.kind);
            //-------------------------------------
            if (AK.nthParameterKind(1) == Kinds[max_i] &&
                AK.nthParameterKind(2) == Kinds[max_i + 1])
            {
                Units[max_i] = MakeBinaryOp(Units[max_i], Operators[max_i], Units[max_i + 1]);//String('(') + Units[max_i] + Operators[max_i] + Units[max_i + 1] + ")";
                Kinds[max_i] = AK.ReturnType();
                Units[max_i + 1] = 0; // delete it
                Kinds[max_i + 1] = 0; // delete it
                Operators[max_i] = 0; // delete it
                ConstructMensoWithOperators(Units, Kinds, Operators); // recursive
            }
            else
                ReportError("mismatch kind for operartor", end);
        }
    }
    else
    {
        std_menso = Units[0];
        kind = Kinds[0];
    }
}
//===================================================
void
ReadingMenso::
Run(Unsigned offset)
{
    List Units;
    List Kinds;
    List Operators;

    end = offset;
    while (true)
    {
        // ReadingUnit RU(ML, end);
        auto MKE = ReadUnit(end);
        // end = RU.end;
        //Units << RU.std_menso;
        Units << MKE.menso;
        Kinds << MKE.kind;
        end = MKE.end;

        String may_be_operator = ML.text_item[end + 1];
        IsOperator IO(ML, may_be_operator);
        if (IO.is_operator == true)
        {
            //offset = end + 1;

            ThingOrOperator T(IO.operator_thing);
            AnalyzingKind AK(T.kind);

            if (AK.ParameterNumber() == 2)
                Operators << may_be_operator;
            else
                ReportError("misplaced single operator", end + 1);
            end = end + 2;
        }
        else
        {
            if (Operators.Size() == 0)
            {
                std_menso = MKE.menso;
                kind = MKE.kind;
            }
            else
                ConstructMensoWithOperators(Units, Kinds, Operators);

            break;
        }
    } // while


    while (true)
    {
        AnalyzingKind AK(kind);
        if (AK.IsFunctional())
        {
            String next = ML.text_item[end + 1];
            if (next != "(")
                return;
            std_menso += "(";

            //ReadingSimpleMenso RSM(ML); // just using ReadingSimpleMenso::GetArgList_And_SetKind method
            //RSM.GetArgList_And_SetKind(kind, end + 2);

            auto MKE = GetArgList_And_Kind(kind, end + 2);
            std_menso += MKE.menso;
            kind = MKE.kind;
            end = MKE.end;
            // std_menso += RSM.std_menso;
            //end = RSM.end;
            //kind = RSM.kind;
        }
        else
            return;
    } // while
} // Run
