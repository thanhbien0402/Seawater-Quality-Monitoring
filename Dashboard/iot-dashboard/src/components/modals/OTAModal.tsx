'use client'
import { useState } from 'react'
import { Button } from '../ui/button'
import {
    Dialog,
    DialogContent,
    DialogDescription,
    DialogHeader,
    DialogTitle,
    DialogTrigger
} from '../ui/dialog'
import { SquarePlus } from 'lucide-react'
import { useTelemetry } from '../context/TelemetryProvider'
import { sendDataTb } from '@/actions'

export function OTAModal({ deviceId, isInMap, deviceName }: { deviceId: string, isInMap?: boolean, deviceName?: string }) {
    const [isOpen, setIsOpen] = useState<boolean>(false)
    const { currentFWVersion, fwVersion, fwTitle } = useTelemetry()

    console.log('currentFW', currentFWVersion, typeof currentFWVersion)

    const token = global?.window?.localStorage.getItem('token') ?? ''

    return (
        <Dialog open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DialogTrigger asChild>
                <Button variant="ghost" className={`gap-1 w-full flex ${!isInMap ? 'justify-between' : 'justify-start'}`} >
                    <SquarePlus className="h-4 w-4" />
                    <p>{isInMap ? deviceName : 'OTA'}</p>
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[425px]">
                <DialogHeader>
                    <DialogTitle>Firmware Update</DialogTitle>
                    {(currentFWVersion === fwVersion)
                        ? `Your version ${currentFWVersion} is up-to-date`
                        : <>
                            <DialogDescription>
                                The new version <span className='text-red-600 text font-bold'>{fwVersion}</span> is available. <span className='text-blue-700 font-bold'>Click to update!</span>
                            </DialogDescription>
                            <Button onClick={async () => {
                                await sendDataTb(fwTitle, 'ota', token, deviceId, fwVersion)
                                setIsOpen(false)
                            }}>Update now</Button>
                        </>
                    }

                </DialogHeader>
            </DialogContent>
        </Dialog >
    )
}
