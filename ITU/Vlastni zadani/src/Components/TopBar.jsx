import './TopBar.css';

import Button from 'react-bootstrap/Button';
import Container from 'react-bootstrap/Container'
import Row from 'react-bootstrap/Row';
import Col from 'react-bootstrap/Col';
import Card from 'react-bootstrap/Card';

import { useHistory } from 'react-router-dom';
import { useState, useEffect } from 'react';
import bank_img from '../Other/bank-1938641-1643400inverted.png'

function TopBar(props) {
    const history = useHistory();   //router history
    const [current_page, setCurrPage] = useState(props.page_num);   //page number state

    const [time, setTime] = useState(props.data.time);
    const [date, setDate] = useState(props.data.date);
    const [left, setLeft] = useState(props.data.left)

    //add singl zero padding to number
    function padTime(num){
        if(num < 10) {
            num = ("0"+num).slice(-2);
        }
        return num;
    }

    //update time every second
    useEffect(() => {
        const interval = setInterval(() => {
            var curr_date = new Date();                                         //get current time
            var diff_ms = Math.abs((new Date(props.data.expiry)) - curr_date);  //get milisecnds difference
            props.data.date = new Date().toLocaleDateString("cs-CS");           //save curr date
            props.data.time = new Date().toLocaleTimeString("cs-CS");           //save curr time
            diff_ms = (diff_ms - (diff_ms % 1000)) / 1000;                      //remove miliseconds
            var secs = diff_ms % 60;                                            //get seconds
            diff_ms = (diff_ms - secs) / 60;                    
            var mins = diff_ms % 60;                                            //get minutes
            var hrs = (diff_ms - mins) / 60;                                    //get hours
            props.data.left = padTime(hrs)+":"+padTime(mins)+":"+padTime(secs); //save expiry time
            setTime(props.data.time);
            setDate(props.data.date);
            setLeft(props.data.left);
        }, 1000);
        return () => clearInterval(interval);
    }, []);

    const onButtonClick = () => history.push({ pathname: "/login" });   //got to login

    //select correct page depending on page number
    const onPageChange = (page_num) => {
        setCurrPage(page_num);
        switch (page_num) {
            case 1: history.push({ pathname: "/home",     data: props.data, page_num: page_num }); break;
            case 2: history.push({ pathname: "/history",  data: props.data, page_num: page_num }); break;
            case 3: history.push({ pathname: "/payments", data: props.data, page_num: page_num }); break;
            //case 4: history.push({ pathname: "/settings", data: props.data, page_num: page_num }); break;
            default: break;
        }
    }

    return (
        <div className="TopBar" style={{"padding":20}}>
            <Container>
                <Row>
                    <Col xs={2} xl>
                        <img style={{"float":"left"}} className="Image" src={bank_img} alt="Bank" height="60"/>
                    </Col>
                    <Col xs={4} xl={2}>
                    <Card className="bg-dark text-light border border-light">
                        <Container>
                        <Row>
                            <Col>
                                <div style={{"textAlign" : "center"}}>
                                    Date: {date}
                                </div>
                            </Col>
                        </Row>
                        <Row>
                            <Col>
                                <div style={{"textAlign" : "center"}}>
                                    Time: {time}
                                </div>
                            </Col>
                        </Row>
                        </Container>
                        </Card>
                    </Col>
                    <Col xs={4} xl={2}>
                        <Card className="bg-dark text-light border border-light">
                            <Row>
                                <Col>
                                    <div style={{"textAlign" : "center"}}>
                                        Certificate expiry:
                                    </div>
                                </Col>
                            </Row>
                            <Row>
                                <Col>
                                    <div style={{"textAlign" : "center"}}>
                                        {left}
                                    </div>
                                </Col>
                            </Row>
                        </Card>
                    </Col>
                    <Col xs={2} xl>
                        <Button className="LOButton" variant="primary" onClick={onButtonClick}>
                            Log out
                        </Button>
                    </Col>
                </Row>
                <div style={{"marginTop": 10}}>
                <Container fluid="true">
                    <Row noGutters>
                        <Col> <Button size="lg" className="HomeBtn              border-left border-top border-bottom rounded-0" variant={current_page === 1 ? "primary" : "secondary"} onClick={() => onPageChange(1)} block >Home    </Button> </Col>
                        <Col> <Button size="lg" className="HistBtn              border-left border-top border-bottom rounded-0" variant={current_page === 2 ? "primary" : "secondary"} onClick={() => onPageChange(2)} block >History </Button> </Col>
                        <Col> <Button size="lg" className="PaymBtn border-right border-left border-top border-bottom rounded-0" variant={current_page === 3 ? "primary" : "secondary"} onClick={() => onPageChange(3)} block >Payments</Button> </Col>
                    </Row>
                </Container>
                </div>
            </Container>
        </div>
    )
}

export default TopBar